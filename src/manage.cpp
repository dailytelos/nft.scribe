ACTION carboncert::sysglobalstr(name &var, string &sval) {
    require_auth( get_self() );

    setglobalstr(var, sval);
}

ACTION carboncert::sysglobalint(name &var, uint64_t &nval) {
    require_auth( get_self() );
    
    //check(var.value != name("certcount").value, "Variable 'certcount' must not be modified.");

    setglobalint(var, nval);
}

ACTION carboncert::sysdefaults() {
    require_auth( get_self() );
    
    delglobal(name("tokencontr"));
    setglobalstr(name("tokencontr"), "coxc.token");

    delglobal(name("tokensymbol"));
    setglobalstr(name("tokensymbol"), "COXC");

    delglobal(name("orgcontract"));
    setglobalstr(name("orgcontract"), "coxc.cert");

    delglobal(name("tokenprec"));
    setglobalint(name("tokenprec"), 4);
    
    delglobal(name("issuefee"));
    setglobalint(name("issuefee"), 4);

    delglobal(name("freeze"));
    setglobalint(name("freeze"), 0);

    if(getglobalint(name("issuefeettl")) == 0) 
    {
        delglobal(name("issuefeettl"));
        setglobalint(name("issuefeettl"), 0);
    }

    if(getglobalint(name("depositacct")) == 0) 
    {
        delglobal(name("depositacct"));
        setglobalint(name("depositacct"), 0);
    }

    if(getglobalint(name("certcount")) == 0) 
    {
        delglobal(name("certcount"));
        setglobalint(name("certcount"), 0);
    }

    if(getglobalint(name("retirecount")) == 0) 
    {
        delglobal(name("retirecount"));
        setglobalint(name("retirecount"), 1);
    }

    if(getglobalint(name("retiredtblct")) == 0) 
    {
        delglobal(name("retiredtblct"));
        setglobalint(name("retiredtblct"), 0);
    }

    if(getglobalint(name("usdval")) == 0) 
    {
        delglobal(name("usdval"));
        setglobalint(name("usdval"), 0);
    }

    if(getglobalint(name("sgdval")) == 0) 
    {
        delglobal(name("sgdval"));
        setglobalint(name("sgdval"), 0);
    }

}

ACTION carboncert::sysdrawacct(name &acct, name &to, asset &quant, std::string &memo) {
    require_auth( get_self() );
    checkfreeze();
    if(acct.value == name("depositacct").value)
    {
        check(memo == "override alpha omega delta two", "This function can not draw from the deposit account, use 'draw' action.");
        memo = "Deposit account draw, this may cause an imbalance in contract totals.  Please verify balances are handled correctly! ";
    }

    check(quant.amount > 0, "Quantity must be greater than 0. ");

    uint64_t bgtacct = getglobalint(acct);
    check(bgtacct > 0, "No funds in account left to withdraw. ");

    uint64_t withdraw = (uint64_t) quant.amount;
    check((bgtacct >= withdraw), "There is not that much to withdraw. ");
    uint64_t new_balance = bgtacct - withdraw;

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

    setglobalint(acct, new_balance);
}

ACTION carboncert::sysdeposit(name &user, asset &quant, string &memo) {
    require_auth( get_self() );
    checkfreeze();
    adddeposit(user, quant, memo);
}

ACTION carboncert::sysdelglobal(name &var) {
    require_auth( get_self() );
    checkfreeze();

    check(var.value != name("certcount").value, "Variable 'certcount' must not be modified.");

    global_index _globals( get_self(), get_self().value );
    auto itr = _globals.find(var.value);

    check(itr != _globals.end(), "Variable (" + var.to_string() + ") not found. ");

    delglobal(var);
}

ACTION carboncert::sysfreeze(uint64_t &freeze) {
    require_auth( get_self() );

    setglobalint(name("freeze"), freeze);
    print("Contract frozen state updated.");
}

ACTION carboncert::draw(name &user) {
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

        uint64_t depositacct = getglobalint(name("depositacct"));
        if(depositacct >= 0 )
        {
            int64_t new_balance = depositacct - drawbal.amount;
            check(new_balance >= 0, "Imbalance occurred, cannot withdraw beyond total in depositacct. (Imb: a) ");
            
            setglobalint(name("depositacct"), new_balance);
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

void carboncert::checkfreeze() {

    uint64_t nfreeze = getglobalint(name("freeze"));

    if(nfreeze == 0) { return; }
    if(nfreeze == 1) //require carboncert to execute
    {
        check(has_auth(get_self()), "carbon.cert is currently undergoing maintenance. ");
        return;
    }
    if(nfreeze >= 2)
    { check(false, "carbon.cert is currently undergoing maintenance. "); }
}

string carboncert::getglobalstr(name var) {
    global_index _globals( get_self(), get_self().value );
    auto itr = _globals.find(var.value);

    if(itr == _globals.end()) { return ""; }

    return itr->sval;
}

void carboncert::setglobalstr(name var, string sval) {

    global_index _globals( get_self(), get_self().value );
    auto itr = _globals.find(var.value);

    if(itr == _globals.end()) {
        //create new record
         _globals.emplace( get_self(), [&]( auto& global_row ) {
            global_row.var  = var;
            global_row.sval   = sval;
            global_row.nval   = 0;

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

uint64_t carboncert::getglobalint(name var) {
    global_index _globals( get_self(), get_self().value );
    auto itr = _globals.find(var.value);

    if(itr == _globals.end()) { return 0; }

    return itr->nval;
}

void carboncert::setglobalint(name var, uint64_t nval) {

    global_index _globals( get_self(), get_self().value );
    auto itr = _globals.find(var.value);

    if(itr == _globals.end()) {
        //create new record
         _globals.emplace( get_self(), [&]( auto& global_row ) {
            global_row.var  = var;
            global_row.sval   = "";
            global_row.nval   = nval;

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

void carboncert::delglobal(name var) {

    global_index _globals( get_self(), get_self().value );
    auto itr = _globals.find(var.value);

	if( itr != _globals.end() ) {
		itr = _globals.erase( itr );
        return;
	}
}

name carboncert::getcontract() {
    string sname = getglobalstr(name("tokencontr"));

    return name(sname);
}

name carboncert::getorgcontract() {
    string sname = getglobalstr(name("orgcontract"));

    return name(sname);
}

void carboncert::adddeposit(name &user, asset &quant, string &memo) {

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
            uint64_t depositacct = (uint64_t) getglobalint(name("depositacct")) + nAmt;
            setglobalint(name("depositacct"), depositacct);
        });
    }
    else { //modify record
        _deposits.modify( itr, get_self(), [&]( auto& deposit_row ) {
            int64_t new_amount = nAmt + deposit_row.quant.amount;
            deposit_row.quant.amount = new_amount;
            deposit_row.memo = "*Multiple* - Last Tran: " + memo;
            uint64_t depositacct = (uint64_t) getglobalint(name("depositacct")) + nAmt;
            setglobalint(name("depositacct"), depositacct);
        });
    }
}


void carboncert::subdeposit(name &user, asset &quant, string &memo) {

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
    }
    else { //modify record
        check((itr->quant.amount - nAmt) > 0, "Subtracting too much deposit, just use deldeposit instead");

        _deposits.modify( itr, get_self(), [&]( auto& deposit_row ) {
            int64_t new_amount = deposit_row.quant.amount - nAmt;
            deposit_row.quant.amount = new_amount;
            deposit_row.memo = "*Multiple* - Last Tran: " + memo;
            check((getglobalint(name("depositacct")) - nAmt) > 0, "Unable to modify deposits, depositacct fell to 0. ");
            uint64_t depositacct = (uint64_t) getglobalint(name("depositacct")) - nAmt;
            setglobalint(name("depositacct"), depositacct);
        });
    }
}

void carboncert::deldeposit(name &user) {

    deposits_index _deposits( get_self(), get_self().value );
    auto itr = _deposits.find(user.value);

    check(itr != _deposits.end(), "Unable to delete deposit, user deposit not found. ");

    itr = _deposits.erase( itr ); //remove row
}

asset carboncert::getdepamt(name &user) {

    deposits_index _deposits( get_self(), get_self().value );
    auto itr = _deposits.find(user.value);

    if(itr == _deposits.end()) {
        return asset(0, symbol(getglobalstr(name("tokensymbol")), getglobalint(name("tokenprec"))));
    } else {
        return itr->quant;
    }
}

string carboncert::getdepmemo(name &user) {

    deposits_index _deposits( get_self(), get_self().value );
    auto itr = _deposits.find(user.value);

    string sRet = "";

    if(itr == _deposits.end()) {
    } else {
        sRet = itr->memo;
    }

    return sRet;
}
