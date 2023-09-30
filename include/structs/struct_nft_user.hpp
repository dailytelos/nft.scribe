//struct_nft_user.hpp

//scope is: suffix from TABLE nftservice in the file nftservice.hpp
struct struct_nft_user {
    name userid;       // Username prefix may be 4 characters, suffixes may be up to 7 characters for a project.  Example: name("1234.suffixx");
    name suffix;       //suffix references the name suffix in TABLE nftservice in the file nftservice.hpp
                       //each NFT project registers a unique suffix under that network_id, so the suffix represents the project

    struct token_entry {
        uint64_t id;
        asset token;

        EOSLIB_SERIALIZE(token_entry, (id)(token));
    };

    vector<token_entry> wallet;                  //stores the user wallet balances for each token

    vector <time_point_sec> a_tps_recent_trx;    //logs tps_created up to GLOBAL_RECENT_TRX_MAX

    struct_nft_user() {
        userid = name(NAME_NULL);
        suffix = name(NAME_NULL);
    }

    struct_nft_user(name i_userid) {
        
        string sUserID = i_userid.to_string();
        vector<string> aUserID = split(sUserID, ".");

        check(aUserID.size() == 2, "Invalid i_userid supplied to struct_nft_user(...). ");
        check((aUserID[0].size() <= 5) && (aUserID[0].size() > 0), "Invalid userid provided to struct_nft_user(...). ");
        check((aUserID[1].size() <= 6) && (aUserID[1].size() > 0), "Invalid suffix provided to struct_nft_user(...). ");

        userid = i_userid;
        suffix = name(aUserID[1]);
    }

    void add_trx(time_point_sec created) {

        verify_user(created);
        a_tps_recent_trx.push_back(created);
    }

    void verify_user(time_point_sec tps_created) {
        check(wallet.size() <= GLOBAL_TOKEN_MAX, "User can't have more than " + std::to_string(GLOBAL_TOKEN_MAX) + " different token types in their balance.");
        check(a_tps_recent_trx.size() <= GLOBAL_RECENT_TRX_MAX, "You have too many recent transactions, the maximum is " + std::to_string(GLOBAL_RECENT_TRX_MAX) + ".");
        
        // Remove timestamps that are older than GLOBAL_MAX_TRX_SECONDS * 2
        auto limit_time = tps_created - seconds(GLOBAL_MAX_TRX_SECONDS * 2);
        a_tps_recent_trx.erase(
            remove_if(a_tps_recent_trx.begin(), a_tps_recent_trx.end(), [limit_time](const time_point_sec& tps) {
                return tps < limit_time;
            }),
            a_tps_recent_trx.end()
        );

        auto found = find(a_tps_recent_trx.begin(), a_tps_recent_trx.end(), tps_created);
        check(found == a_tps_recent_trx.end(), "Timestamp already exists in recent transactions.");
    }

    uint32_t get_prefix_number() {
        return (uint32_t)name(get_prefix_as_string()).value;
    }

    string get_prefix_as_string() {
        vector<string> aRet = split(userid.to_string(), ".");
        check(aRet.size() == 2, "Invalid userid name found when get_prefix_as_string(...) called.");
        check(aRet[0].size() <= 4, "Invalid prefix size found when get_prefix_as_string(...) called.");

        return aRet[0];
    }

    void add_token_to_balance(uint64_t id, asset token_asset) {
        check(token_asset.amount > 0, "add_token_to_balance - Token amount must be positive: " + token_asset.to_string());

        auto existing_token_itr = find_if(wallet.begin(), wallet.end(), [&](const token_entry& t) {
            return t.id == id;
        });

        if (existing_token_itr == wallet.end()) {
            check(wallet.size() < GLOBAL_TOKEN_MAX, "User can't have more than " + std::to_string(GLOBAL_TOKEN_MAX) + " different token types in their balance.");
            wallet.push_back({id, token_asset});
        } else {
            check(existing_token_itr->token.symbol == token_asset.symbol, "Token symbol or precision mismatch.");
            existing_token_itr->token += token_asset;
        }
    }

    void sub_token_from_balance(uint64_t id, asset token_asset) {
        check(token_asset.amount > 0, "sub_token_from_balance - Token amount must be positive: " + token_asset.to_string());

        auto existing_token_itr = find_if(wallet.begin(), wallet.end(), [&](const token_entry& t) {
            return t.id == id;
        });

        check(existing_token_itr != wallet.end(), "Token ID not found in user's balance.");
        check(existing_token_itr->token.symbol == token_asset.symbol, "Token symbol or precision mismatch.");

        existing_token_itr->token -= token_asset;
        check(existing_token_itr->token.amount >= 0, "Insufficient token balance.");

        if (existing_token_itr->token.amount == 0) {
            wallet.erase(existing_token_itr);
        }
    }

    vector<string> split(string str, string token){
        vector<string>result;
        while(str.size()){
            int index = str.find(token);
            if(index!=string::npos){
                result.push_back(str.substr(0,index));
                str = str.substr(index+token.size());
                if(str.size()==0)result.push_back(str);
            }else{
                result.push_back(str);
                str = "";
            }
        }
        return result;
    }

    EOSLIB_SERIALIZE(struct_nft_user, (userid)(suffix)(wallet)(a_tps_recent_trx));
};