//transfer.hpp

public:

[[eosio::on_notify("*::transfer")]] 
void on_transfer(name from, name to, asset quant, std::string memo) {
    
    //check(get_first_receiver() == "eosio.token"_n, "should be eosio.token");
    //check(false, "Transfers are refused.");

    name contract = get_first_receiver();
    int64_t nAmt = quant.amount;
    symbol_code cUnit = quant.symbol.code();
    string sUnit = cUnit.to_string();
    uint8_t nPrec = quant.symbol.precision();

    check(nAmt > 0, "Insufficient funds sent. ");

    //*** Perform checks based on memo *******************************
    if( ((memo.substr(0,12) == "sysdeposit!!")&&(memo.size() == 12)) &&  // for depositing system token into the contract
        (to.value == get_self().value) &&
        (contract == getcontract()) &&
        (sUnit == getglobalstr(name("tokensymbol"))) ) {
        
        checkfreeze();

        string sMemo = from.to_string() + " deposited " + quant.to_string() + " into the contract under sysdeposit. ";

        adddeposit(from, quant, sMemo);

    } else if(to.value == get_self().value) {
        // Other Types
        // memo string will be formatted as follows userid@network_id
        // Example:  "ad42.sfx@eth.mainnet"

        string aSplit = split(memo, "@");
        check(aSplit.size() == 2, "Invalid memo specified, memos should specify a virtual account user like: ad42.sfx@eth.mainnet ");

        name network_id = name(aSplit[1]);
        name userid = name(aSplit[0]);

        string aSplit2 = split(aSplit[0], ".");
        check(aSplit2.size() == 2, "Invalid memo specified, memos should specify a virtual account user like: ad42.sfx@eth.mainnet ");

        name suffix = name(aSplit2[1]);

        // secondary key by_contract_and_symbol() to search TABLE tokenstbl in tokens.hpp
        uint128_t combined_key = (static_cast<uint128_t>(contract.value) << 64) | static_cast<uint128_t>(symbol_code(cUnit).raw());

        // Check for matching token using the secondary key
        auto existing_token = _tokens_table.get_index<name("contractsym")>();
        auto token_itr = existing_token.find(combined_key);
        check(token_itr != existing_token.end(), "Token cannot be sent to this contract, it was not registered to the list of accepted tokens. ");

        struct_token cToken = struct_token(token_itr->t.id, contract, quant, sUnit, nPrec);

        _nftuser_token_transfer_in(network_id, userid, cToken, memo);
    }
}

private:
