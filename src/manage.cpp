//manage.cpp

ACTION nftscribe::sysglobalstr(name &var, string &sval) {
    require_auth( get_self() );

    setglobalstr(var, sval);
}

ACTION nftscribe::sysglobalint(name &var, uint64_t &nval) {
    require_auth( get_self() );
    
    setglobalint(var, nval);
}

ACTION nftscribe::sysglobalast(name &var, asset &aval) {
    require_auth( get_self() );
    
    setglobalast(var, aval);
}

ACTION nftscribe::sysdefaults() {
    require_auth( get_self() );
    
    //used in checkfreeze - Global contract freezing functionality
    // 1 - Freezes all public contract activity
    // 2 - Freezes all admin & public contract activity,
    //        only global variable functions remain open
    delglobal(name("freeze"));
    setglobalint(name("freeze"), 1);

    // For system deposits / withdrawals
    // Relates to payments made directly to this contract in TLOS.
    delglobal(name("tokentbl.id"));
    setglobalint(name("tokentbl.id"), 1);

    delglobal(name("tokencontr"));
    setglobalstr(name("tokencontr"), "eosio.token");

    delglobal(name("tokensymbol"));
    setglobalstr(name("tokensymbol"), "TLOS");

    delglobal(name("tokenprec"));
    setglobalint(name("tokenprec"), 4);

    // add the above system token
    //_sysaddtoken(...);

    if(getglobalast(name("depositacct")).amount == 0) 
    {
        delglobal(name("depositacct"));
        setglobalast(name("depositacct"), system_asset(0));
    }

    //Fees and Charges - Default Setup
    delglobal(name("fee.nftserv"));
    setglobalast(name("fee.nftserv"), system_asset(500000)); // 50 TLOS initial fee

    //Oracle Balance Account - Collected Fees to Pay Oracles
    if(getglobalast(name("acct.oracles")).amount == 0) {
        delglobal(name("acct.oracles"));
        setglobalast(name("acct.oracles"), system_asset(0));
    }

    //Oracle tier 1 - BP with more than 34 mil votes
    delglobal(name("orc.tierone"));
    setglobalint(name("orc.tierone"), 34000000);

    //Oracle tier 2 - BP with more than 54 mil votes
    delglobal(name("orc.tiertwo"));
    setglobalint(name("orc.tiertwo"), 54000000);
}

ACTION nftscribe::sysdrawacct(name &acct, name &to, asset &quant, std::string &memo) {
    require_auth( get_self() );
    checkfreeze();
    if(acct.value == name("depositacct").value)
    {
        check(memo == "override alpha omega delta two", "This function can not draw from the deposit account, use 'draw' action.");
        memo = "Deposit account draw, this may cause an imbalance in contract totals.  Please verify balances are handled correctly! ";
    }

    check(quant.amount > 0, "Quantity must be greater than 0. ");

    asset bgtacct = getglobalast(acct);
 
        //check if assets match
        check(bgtacct.symbol.code().to_string() == getglobalstr(name("tokensymbol")), "Invalid symbol_code for bgtacct. ");
        check(bgtacct.symbol.code().to_string() == quant.symbol.code().to_string(), "Invalid symbol_code for quant. ");
        
        check(bgtacct.symbol.precision() == getglobalint(name("tokenprec")), "Invalid precision for bgtacct. ");
        check(bgtacct.symbol.precision() == quant.symbol.precision(), "Invalid precision for quant. ");
        
    check(bgtacct.amount > 0, "No funds in account left to withdraw. ");

    int64_t withdraw = quant.amount;
    check((bgtacct.amount >= withdraw), "There is not that much to withdraw. ");
    int64_t new_balance = bgtacct.amount - withdraw;

    action(
    permission_level{ get_self(), "active"_n},
    getcontract(),
    "transfer"_n,
    std::make_tuple(
        get_self(),
        to,
        quant,
        memo
        )
    ).send();

    setglobalast(acct, system_asset(new_balance));
}

ACTION nftscribe::sysdeposit(name &user, asset &quant, string &memo) {
    require_auth( get_self() );
    checkfreeze();
    adddeposit(user, quant, memo);
}

ACTION nftscribe::sysdelglobal(name &var) {
    require_auth( get_self() );
    checkfreeze();

    global_index _globals( get_self(), get_self().value );
    auto itr = _globals.find(var.value);

    check(itr != _globals.end(), "Variable (" + var.to_string() + ") not found. ");

    delglobal(var);
}

ACTION nftscribe::sysfreeze(uint64_t &freeze) {
    require_auth( get_self() );

    setglobalint(name("freeze"), freeze);
    print("Contract frozen state updated.");
}

ACTION nftscribe::draw(name &user) {
    check(has_auth(user) || has_auth(get_self()), "Transaction missing required authority. ");
    checkfreeze();
    deposits_index _deposits( get_self(), get_self().value );
    auto itr = _deposits.find(user.value);

    check(itr != _deposits.end(), "No deposits found associated with account. ");

    if(itr != _deposits.end()) {
        asset drawbal = itr->quant;
        check(drawbal.amount > 0, "Balance in account must be greater than 0. ");
        symbol_code cUnit = drawbal.symbol.code();
        string sUnit = cUnit.to_string();
        uint8_t nPrec = drawbal.symbol.precision();
        check(sUnit == getglobalstr(name("tokensymbol")), "Balance account error, mismatch in symbol. ");
        check(nPrec == getglobalint(name("tokenprec")), "Balance account error, mismatch in precision. ");

        asset depositacct = getglobalast(name("depositacct"));
        if(depositacct.amount >= 0 )
        {
            asset new_balance = system_asset(depositacct.amount - drawbal.amount);
            check(new_balance.amount >= 0, "Imbalance occurred, cannot withdraw beyond total in depositacct. (Imb: a) ");
            
            setglobalast(name("depositacct"), new_balance);
        }
        else {
            check(false, "Imbalance occurred, cannot withdraw beyond total in depositacct. (Imb: b) ");
        }

        action(
            permission_level{ get_self(), "active"_n},
            getcontract(),
            "transfer"_n,
            std::make_tuple(
            get_self(),
            itr->user,
            drawbal,
            itr->memo
            )
        ).send();

        itr = _deposits.erase( itr ); //remove row
    }
}

//********************************************
// ************* private internal functions ***
//********************************************

void nftscribe::checkfreeze() {

    uint64_t nfreeze = getglobalint(name("freeze"));

    if(nfreeze == 0) { return; }
    if(nfreeze == 1) //require nftscribe to execute
    {
        check(has_auth(get_self()), get_self().to_string() + " is currently undergoing maintenance. ");
        return;
    }
    if(nfreeze >= 2)
    { check(false, get_self().to_string() + " is currently undergoing maintenance. "); }
}

string nftscribe::getglobalstr(name var) {
    global_index _globals( get_self(), get_self().value );
    auto itr = _globals.find(var.value);

    if(itr == _globals.end()) { return ""; }

    return itr->sval;
}

void nftscribe::setglobalstr(name var, string sval) {

    global_index _globals( get_self(), get_self().value );
    auto itr = _globals.find(var.value);

    if(itr == _globals.end()) {
        //create new record
         _globals.emplace( get_self(), [&]( auto& global_row ) {
            global_row.var  = var;
            global_row.sval   = sval;
            global_row.nval   = 0;
            global_row.aval   = asset(0, symbol(symbol_code("NULL"), 4));

            //print("Variable added: " + var.to_string() + " - " + sval);
        });
    }
    else { //modify record
        _globals.modify( itr, get_self(), [&]( auto& global_row ) {
            global_row.sval = sval;

            //print("Variable changed: " + var.to_string() + " - " + sval);
        });
    }
}

uint64_t nftscribe::getglobalint(name var) {
    global_index _globals( get_self(), get_self().value );
    auto itr = _globals.find(var.value);

    if(itr == _globals.end()) { return 0; }

    return itr->nval;
}

void nftscribe::setglobalint(name var, uint64_t nval) {

    global_index _globals( get_self(), get_self().value );
    auto itr = _globals.find(var.value);

    if(itr == _globals.end()) {
        //create new record
         _globals.emplace( get_self(), [&]( auto& global_row ) {
            global_row.var  = var;
            global_row.sval   = "";
            global_row.nval   = nval;
            global_row.aval   = asset(0, symbol(symbol_code("NULL"), 4));

            //print("Variable added: " + var.to_string() + " - " + to_string(nval));
        });
    }
    else { //modify record
        _globals.modify( itr, get_self(), [&]( auto& global_row ) {
            global_row.nval = nval;

            //print("Variable changed: " + var.to_string() + " - " + to_string(nval));
        });
    }
}


asset nftscribe::getglobalast(name var) {
    global_index _globals( get_self(), get_self().value );
    auto itr = _globals.find(var.value);

    if(itr == _globals.end()) { return asset(0, symbol(symbol_code("NULL"), 4)); }

    return itr->aval;
}

void nftscribe::setglobalast(name var, asset aval) {

    global_index _globals( get_self(), get_self().value );
    auto itr = _globals.find(var.value);

    if(itr == _globals.end()) {
        //create new record
         _globals.emplace( get_self(), [&]( auto& global_row ) {
            global_row.var  = var;
            global_row.sval   = "";
            global_row.nval   = 0;
            global_row.aval   = aval;

            //print("Variable added: " + var.to_string() + " - " + to_string(nval));
        });
    }
    else { //modify record
        _globals.modify( itr, get_self(), [&]( auto& global_row ) {
            global_row.aval = aval;

            //print("Variable changed: " + var.to_string() + " - " + to_string(nval));
        });
    }
}


void nftscribe::delglobal(name var) {

    global_index _globals( get_self(), get_self().value );
    auto itr = _globals.find(var.value);

	if( itr != _globals.end() ) {
		itr = _globals.erase( itr );
        return;
	}
}

name nftscribe::getcontract() {
    string sname = getglobalstr(name("tokencontr"));

    return name(sname);
}

void nftscribe::adddeposit(const name &user, const asset &quant, const string &memo) {

    int64_t nAmt = quant.amount;
    symbol_code cUnit = quant.symbol.code();
    string sUnit = cUnit.to_string();
    uint8_t nPrec = quant.symbol.precision();

    check(nAmt > 0, "Amount in quant must be greater than 0. ");
    check(sUnit == getglobalstr(name("tokensymbol")), "Contract presently only works with " + getglobalstr(name("tokensymbol")) + " token. ");
    check(nPrec == getglobalint(name("tokenprec")), "Precision error in given value quant. ");
    check(memo.size() <= 220, "Memo is too large, limit of 220 characters for adddeposit(...). ");

    deposits_index _deposits( get_self(), get_self().value );
    auto itr = _deposits.find(user.value);

    if(itr == _deposits.end()) {
         _deposits.emplace( get_self(), [&]( auto& deposit_row ) {
            deposit_row.user     = user;
            deposit_row.quant    = quant;
            deposit_row.memo     = memo;
            asset depositacct = system_asset(getglobalast(name("depositacct")).amount + nAmt);
            setglobalast(name("depositacct"), depositacct);
        });
    }
    else { //modify record
        _deposits.modify( itr, get_self(), [&]( auto& deposit_row ) {
            int64_t new_amount = nAmt + deposit_row.quant.amount;
            deposit_row.quant.amount = new_amount;
            deposit_row.memo = "*Multiple* - Last Tran: " + memo;
            asset depositacct = system_asset(getglobalast(name("depositacct")).amount + nAmt);
            setglobalast(name("depositacct"), depositacct);
        });
    }
}


void nftscribe::subdeposit(const name &user, const asset &quant) {

    int64_t nAmt = quant.amount;
    symbol_code cUnit = quant.symbol.code();
    string sUnit = cUnit.to_string();
    uint8_t nPrec = quant.symbol.precision();

    check(nAmt > 0, "Amount in quant must be greater than 0. ");
    check(sUnit == getglobalstr(name("tokensymbol")), "Contract presently only works with " + getglobalstr(name("tokensymbol")) + " token. ");
    check(nPrec == getglobalint(name("tokenprec")), "Precision error in given value quant. ");
    //check(memo.size() <= 220, "Memo is too large, limit of 220 characters for adddeposit(...). ");

    deposits_index _deposits( get_self(), get_self().value );
    auto itr = _deposits.find(user.value);

    if(itr == _deposits.end()) {
    }
    else { //modify record
        check((itr->quant.amount - nAmt) > 0, "Subtracting too much deposit, just use deldeposit instead");

        _deposits.modify( itr, get_self(), [&]( auto& deposit_row ) {
            int64_t new_amount = deposit_row.quant.amount - nAmt;
            deposit_row.quant.amount = new_amount;
            //deposit_row.memo = "*Multiple* - Last Tran: " + memo;
            check((getglobalast(name("depositacct")).amount - nAmt) > 0, "Unable to modify deposits, depositacct fell to 0. ");
            asset depositacct = system_asset(getglobalast(name("depositacct")).amount - nAmt);
            setglobalast(name("depositacct"), depositacct);
        });
    }
}

void nftscribe::deldeposit(const name &user) {

    deposits_index _deposits( get_self(), get_self().value );
    auto itr = _deposits.find(user.value);

    check(itr != _deposits.end(), "Unable to delete deposit, user deposit not found. ");

    itr = _deposits.erase( itr ); //remove row
}

asset nftscribe::getdepamt(const name &user) {

    deposits_index _deposits( get_self(), get_self().value );
    auto itr = _deposits.find(user.value);

    if(itr == _deposits.end()) {
        return asset(0, symbol(getglobalstr(name("tokensymbol")), getglobalint(name("tokenprec"))));
    } else {
        return itr->quant;
    }
}

string nftscribe::getdepmemo(const name &user) {

    deposits_index _deposits( get_self(), get_self().value );
    auto itr = _deposits.find(user.value);

    string sRet = "";

    if(itr == _deposits.end()) {
    } else {
        sRet = itr->memo;
    }

    return sRet;
}

asset nftscribe::system_asset(int64_t amount) {
    symbol_code sym_code = symbol_code(getglobalstr(name("tokensymbol")));
    uint8_t prec         = getglobalint(name("tokenprec"));

    return asset(amount, symbol(sym_code, prec));
}