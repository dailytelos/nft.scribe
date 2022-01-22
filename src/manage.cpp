ACTION carboncert::sysglobalstr(name &var, string &sval) {
    require_auth( get_self() );

    setglobalstr(var, sval);
}

ACTION carboncert::sysglobalint(name &var, uint64_t &nval) {
    require_auth( get_self() );
    
    check(var.value != name("certcount").value, "Variable 'certcount' must not be modified.");

    setglobalint(var, nval);
}

ACTION carboncert::sysdefaults() {
    require_auth( get_self() );
    
    delglobal(name("tokencontr"));
    setglobalstr(name("tokencontr"), "carbon.token");

    delglobal(name("tokensymbol"));
    setglobalstr(name("tokensymbol"), "XXXX");

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

    if(getglobalint(name("refundacct")) == 0) 
    {
        delglobal(name("refundacct"));
        setglobalint(name("refundacct"), 0);
    }

    if(getglobalint(name("certcount")) == 0) 
    {
        delglobal(name("certcount"));
        setglobalint(name("certcount"), 0);
    }
}

ACTION carboncert::sysdrawacct(name &acct, name &to, asset &quant, std::string &memo) {
    require_auth( get_self() );
    checkfreeze();
    if(acct.value == name("refundacct").value)
    {
        check(memo == "override alpha omega delta two", "This function can not draw from the refund account, use 'refund' action.");
        memo = "Refund account draw, this may cause an imbalance in contract totals.  Please verify balances are handled correctly! ";
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

ACTION carboncert::sysaddrefund(name &user, asset &quant, string &memo) {
    require_auth( get_self() );
    checkfreeze();
    addrefund(user, quant, memo);
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

ACTION carboncert::refund(name &user) {
    check(has_auth(user) || has_auth(get_self()), "Transaction missing required authority. ");
    checkfreeze();
    refunds_index _refunds( get_self(), get_self().value );
    auto itr = _refunds.find(user.value);

    check(itr != _refunds.end(), "No refunds found associated with account. ");

    if(itr != _refunds.end()) {
        asset refundbal = itr->quant;
        check(refundbal.amount > 0, "Balance in account must be greater than 0. ");
        symbol_code cUnit = refundbal.symbol.code();
        string sUnit = cUnit.to_string();
        uint8_t nPrec = refundbal.symbol.precision();
        check(sUnit == getglobalstr(name("tokensymbol")), "Balance account error, mismatch in symbol. ");
        check(nPrec == getglobalint(name("tokenprec")), "Balance account error, mismatch in precision. ");

        uint64_t refundacct = getglobalint(name("refundacct"));
        if(refundacct >= 0 )
        {
            int64_t new_balance = refundacct - refundbal.amount;
            check(new_balance >= 0, "Imbalance occurred, cannot withdraw beyond total in refundacct. (Imb: a) ");
            
            setglobalint(name("refundacct"), new_balance);
        }
        else {
            check(false, "Imbalance occurred, cannot withdraw beyond total in refundacct. (Imb: b) ");
        }

        action(
            permission_level{ get_self(), "active"_n},
            getcontract(),
            "transfer"_n,
            std::make_tuple(
            get_self(),
            itr->user,
            refundbal,
            itr->memo
            )
        ).send();

        itr = _refunds.erase( itr ); //remove row
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

void carboncert::addrefund(name &user, asset &quant, string &memo) {

    int64_t nAmt = quant.amount;
    symbol_code cUnit = quant.symbol.code();
    string sUnit = cUnit.to_string();
    uint8_t nPrec = quant.symbol.precision();

    check(nAmt > 0, "Amount in quant must be greater than 0. ");
    check(sUnit == getglobalstr(name("tokensymbol")), "Contract presently only works with " + getglobalstr(name("tokensymbol")) + " token. ");
    check(nPrec == getglobalint(name("tokenprec")), "Precision error in given value quant. ");
    check(memo.size() <= 220, "Memo is too large, limit of 220 characters for addrefund(...). ");

    refunds_index _refunds( get_self(), get_self().value );
    auto itr = _refunds.find(user.value);

    if(itr == _refunds.end()) {
         _refunds.emplace( get_self(), [&]( auto& refund_row ) {
            refund_row.user     = user;
            refund_row.quant    = quant;
            refund_row.memo     = memo;
            uint64_t refundacct = (uint64_t) getglobalint(name("refundacct")) + nAmt;
            setglobalint(name("refundacct"), refundacct);
        });
    }
    else { //modify record
        _refunds.modify( itr, get_self(), [&]( auto& refund_row ) {
            int64_t new_amount = nAmt + refund_row.quant.amount;
            refund_row.quant.amount = new_amount;
            refund_row.memo = "*Multiple* - Last Tran: " + memo;
            uint64_t refundacct = (uint64_t) getglobalint(name("refundacct")) + nAmt;
            setglobalint(name("refundacct"), refundacct);
        });
    }
}