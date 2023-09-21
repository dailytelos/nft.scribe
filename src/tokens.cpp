//tokens.cpp

    ACTION nftscribe::sysaddtoken(const uint64_t& id, const name& contract, const string& sym_code, const uint8_t& prec) {
        require_auth(get_self());

        _sysaddtoken(id, contract, sym_code, prec);
    }

    ACTION nftscribe::sysdeltoken(const uint64_t& id) {
        require_auth(get_self());

        _sysdeltoken(id);
    }

    void nftscribe::_sysaddtoken(const uint64_t& id, const name& contract, const string& sym_code, const uint8_t& prec) {

        check(is_account(contract), "Specified contract account does not exist, unable to add token.");
        
        name scope = get_self();
        tokens_index _tokens_table( get_self(), scope.value );

        // Create the secondary key from the provided contract and symbol
        uint128_t combined_key = (static_cast<uint128_t>(contract.value) << 64) | static_cast<uint128_t>(symbol_code(sym_code).raw());

        // Check for existing tokens with the same secondary key
        auto existing_token = _tokens_table.get_index<name("contractsym")>();
        auto token_itr = existing_token.find(combined_key);
        check(token_itr == existing_token.end(), "Token with the same contract and symbol already exists.");

        auto primary_itr = _tokens_table.find(id);
        if(primary_itr == _tokens_table.end()){
            _tokens_table.emplace( get_self(), [&]( auto& row ) {
                row.id = id;
                
                struct_token cToken;
                cToken.id = id;
                cToken.contract = contract;
                cToken.token = asset(0, symbol(symbol_code(sym_code), prec));
                row.t = cToken;
            });
        } else { //update existing registration

            check(primary_itr->t.token.amount == 0, "Token holds a non-zero balance, unable to modify.");

            _tokens_table.modify( primary_itr, get_self(), [&]( auto& row ) {

                struct_token cToken;
                cToken.id = id;
                cToken.contract = contract;
                cToken.token = asset(0, symbol(symbol_code(sym_code), prec));
                row.t = cToken;
            });
        }
    }

    void nftscribe::_sysdeltoken(const uint64_t& id) {

        name scope = get_self();
        tokens_index _tokens_table( get_self(), scope.value );
        auto tokens_itr = _tokens_table.find(id);

        if(tokens_itr == _tokens_table.end()){
            check(false, "Token does not exist, unable to delete. ");
        }

        check(tokens_itr->t.token.amount == 0, "Token holds a non-zero balance, unable to delete. ");

        tokens_itr = _tokens_table.erase( tokens_itr ); //remove row
    }

    void nftscribe::_incr_global_token_bal(const uint64_t& id, const asset& token_val) {
        eosio::check(token_val.amount > 0, "Token amount must be positive");

        tokens_index tokens(_self, _self.value); 
        auto itr = tokens.find(id);


        eosio::check(itr != tokens.end(), "Token with the given ID does not exist");
        eosio::check(token_val.symbol == itr->t.token.symbol, "Mismatched token symbols");

        tokens.modify(itr, _self, [&](auto& t) {
            t.t.token.amount += token_val.amount;
        });
    }

    void nftscribe::_decr_global_token_bal(const uint64_t& id, const asset& token_val) {
        eosio::check(token_val.amount > 0, "Token amount must be positive");

        tokens_index tokens(_self, _self.value); 
        auto itr = tokens.find(id);

        eosio::check(itr != tokens.end(), "Token with the given ID does not exist");
        eosio::check(token_val.symbol == itr->t.token.symbol, "Mismatched token symbols");
        eosio::check(itr->t.token.amount >= token_val.amount, "Insufficient balance for the operation");

        tokens.modify(itr, _self, [&](auto& t) {
            t.t.token.amount -= token_val.amount;
        });
    }

    nftscribe::struct_token nftscribe::gettoken(const uint64_t& id) {

        name scope = get_self();
        tokens_index _tokens_table( get_self(), scope.value );
        auto tokens_itr = _tokens_table.find(id);

        if(tokens_itr == _tokens_table.end()){
            check(false, "Token does not exist, unable to fetch its data. ");
        }

        struct_token cToken;
        cToken = tokens_itr->t;

        return cToken;
    }