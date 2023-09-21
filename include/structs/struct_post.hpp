//struct_post.hpp

// The structure for describing an oracle post
struct struct_post {
    uint64_t id;        
    name network_id;    //network id, example: "eth.mainnet", also functions as the scope of the post
    name suffix;        //the suffix denoting the project under which this was posted
    name contract;      //the target contract for the post to interact with
    name post_action;   //post action supplied by oracle
    name userid;        //userid in TABLE nftusers

    name posted_by;     //oracle_id that posted

    uint16_t sign_type; //set variable to signify the signing type, in future could support multiple ways to sign data by oracles
                        // or if different wallets support different versions of signing, etc.
                        // Supported Values:
                        // 4 = "eth_signtypeddata_v4" : https://docs.metamask.io/wallet/reference/eth_signtypeddata_v4/

    string pub_key;     //public key from chain
    uint128_t nft_id;   //ERC721 / etc. ownership claimed by pub_key, calling: contract.ownerOf(nft_id_number)

    string unsigned_data;  //unsigned data
    string signed_data;    //signed data

    vector <name> upvotes;   //oracle upvotes
    vector <name> downvotes; //oracle downvotes
    
    time_point_sec tps_posted;      //date when oracle posted to the blockchain
    time_point_sec tps_created;     //date when the signor claimed they signed in metamask / wallet
    time_point_sec tps_expires;     //post expires after this time, set by the signor

    struct_post() {
        id           = 0;
        network_id   = name(NAME_NULL);
        suffix       = name(NAME_NULL);
        contract     = name(NAME_NULL);
        post_action  = name(NAME_NULL);
        userid       = name(NAME_NULL);
        posted_by    = name(NAME_NULL);

        sign_type     = 0;
        pub_key       = "";
        nft_id        = 0;
        unsigned_data = "";
        signed_data   = ""; 

        tps_posted   = time_point_sec(0);
        tps_created  = time_point_sec(0);
        tps_expires  = time_point_sec(0);
    };

    struct_post(uint64_t i_id, name i_network_id, name i_suffix, name i_contract, name i_post_action, name i_userid, name i_posted_by, uint16_t i_sign_type, string i_pub_key, uint128_t i_nft_id, string i_unsigned_data, string i_signed_data, vector <name> i_upvotes, vector <name> i_downvotes, time_point_sec i_tps_posted, time_point_sec i_tps_created, time_point_sec i_tps_expires) {
        id           = i_id;
        network_id   = i_network_id;
        suffix       = i_suffix;
        contract     = i_contract;
        post_action  = i_post_action;
        userid       = i_userid;
        posted_by    = i_posted_by;

        sign_type     = i_sign_type;
        pub_key       = i_pub_key;
        nft_id        = i_nft_id;
        unsigned_data = i_unsigned_data;
        signed_data   = i_signed_data; 

        upvotes   = i_upvotes;
        downvotes = i_downvotes;

        tps_posted   = i_tps_posted;
        tps_created  = i_tps_created;
        tps_expires  = i_tps_expires;
    };
        
    void upvote(name i_oracle_id) {
        // Check if i_oracle_id is already in the upvotes vector
        auto itr = std::find(upvotes.begin(), upvotes.end(), i_oracle_id);
        if (itr == upvotes.end()) {
            upvotes.push_back(i_oracle_id);
        } else {
            check(false, "Oracle has already upvoted this post.");
        }

        // Make sure to also remove the i_oracle_id from the downvotes vector if it exists there
        auto down_itr = std::find(downvotes.begin(), downvotes.end(), i_oracle_id);
        if (down_itr != downvotes.end()) {
            downvotes.erase(down_itr);
        }
    }

    void downvote(name i_oracle_id) {
        // Check if i_oracle_id is already in the downvotes vector
        auto itr = std::find(downvotes.begin(), downvotes.end(), i_oracle_id);
        if (itr == downvotes.end()) {
            downvotes.push_back(i_oracle_id);
        } else {
            check(false, "Oracle has already downvoted this post.");
        }

        // Make sure to also remove the i_oracle_id from the upvotes vector if it exists there
        auto up_itr = std::find(upvotes.begin(), upvotes.end(), i_oracle_id);
        if (up_itr != upvotes.end()) {
            upvotes.erase(up_itr);
        }
    }
    
    //-------------------------------------------
    // Verification of Data in Post
    //-------------------------------------------

    void verify_network() {  //verifies network id inside unsigned_data matches network_id
        string s_network_id = parse_json(unsigned_data, "network_id");

        check(s_network_id == network_id.to_string(), "network_id has failed to verify #1. ");
        check(name(s_network_id).value == network_id.value, "network_id has failed to verify #2. ");
    }

    void verify_project() { //verifies the nft_contract in unsigned_data matches suffix + network_id contract data
        string s_suffix = parse_json(unsigned_data, "suffix");

        check(s_suffix == suffix.to_string(), "suffix has failed to verify #1. ");
        check(name(s_suffix).value == suffix.value, "suffix has failed to verify #2. ");
    }

    void verify_contract() { //verifies the contract target of the post in unsigned_data
        string s_contract = parse_json(unsigned_data, "contract");

        check(s_contract == contract.to_string(), "contract has failed to verify #1. ");
        check(name(s_contract).value == contract.value, "contract has failed to verify #2. ");
    }

    void verify_post_action() { //verifies the post_action in unsigned_data matches 
        string s_post_action = parse_json(unsigned_data, "post_action");

        check(s_post_action == post_action.to_string(), "post_action has failed to verify #1. ");
        check(name(s_post_action).value == post_action.value, "post_action has failed to verify #2. ");
    }

    void verify_userid() { //verifies the post_action in unsigned_data matches 
        string s_userid = parse_json(unsigned_data, "userid");

        check(s_userid == userid.to_string(), "userid has failed to verify #1. ");
        check(name(s_userid).value == userid.value, "userid has failed to verify #2. ");
    }

    void verify_created() {
        string s_tps_created = parse_json(unsigned_data, "tps_created");

        check(std::stoi(s_tps_created) == tps_created.sec_since_epoch(), "verify_created() failed, invalid data supplied by oracle. ");
    }

    void verify_expires() {
        string s_tps_expires = parse_json(unsigned_data, "tps_expires");

        check(std::stoi(s_tps_expires) == tps_expires.sec_since_epoch(), "verify_expires() failed, invalid data supplied by oracle. ");
    }

    void verify_time() {
                            //verifies times supplied in unsigned_data fall within:
                            //  1) tps_posted must be no later than X seconds after tps_created (global: tps.generate)
                            //  2) tps_posted must be after tps_created
                            //  3) tps_expires must be set within X seconds after tps_created (global: tps.expirelt)
                            //  4) if current time is after tps_expires, transaction is invalidated
                            //  5) tps_created must be before or equal to now

        uint32_t       n_now = current_time_point().sec_since_epoch();

        //#1
        check((tps_created.sec_since_epoch() + GLOBAL_MAX_TRX_SECONDS) >= tps_posted.sec_since_epoch(), "Post generated in wrong timeframe. ");
        //#2
        check(tps_posted.sec_since_epoch() >= tps_created.sec_since_epoch(), "Post data invalid, time created must preceed or match the time of posting. ");
        //#3
        check((tps_created.sec_since_epoch() + GLOBAL_MAX_TRX_SECONDS) >= tps_expires.sec_since_epoch(), "Post expiration invalid. ");
        //#4
        check(n_now < tps_expires.sec_since_epoch(), "Post transaction expired. ");
        //#5
        check(n_now >= tps_created.sec_since_epoch(), "Future transactions are invalid. ");
    }

    void verify() {
        verify_network();
        verify_project();
        verify_contract();
        verify_post_action();
        verify_userid();
        verify_created();
        verify_expires();
        verify_time();
    }

    // parse_json needs debug / needs testing
    //parses fields from JSON, trims white space, return characters, and removes quote characters
    string parse_json(const string& json_data, const string& field) {

        // Parse the JSON by finding the "name" field
        int16_t name_pos = json_data.find("\"" + field + "\"");
        if (name_pos == string::npos)
            return "";

        // Find the beginning of the value (after ":") for the "name" field
        int16_t value_start = json_data.find(":", name_pos);
        if (value_start == string::npos)
            return "";

        // Find the end of the "name" value (up to the next comma or end of JSON object)
        int16_t value_end = json_data.find_first_of(",}", value_start);
        if (value_end == string::npos)
            return "";

        string result = json_data.substr(value_start + 1, value_end - value_start - 1);

        // Trim leading and trailing whitespaces, newline, return characters, and remove quote characters
        auto wsfront = find_if_not(result.begin(), result.end(), [](int c) {
            return isspace(c) || c == '\n' || c == '\r' || c == '\"' || c == '\'';
        });
        auto wsback = find_if_not(result.rbegin(), result.rend(), [](int c) {
            return isspace(c) || c == '\n' || c == '\r' || c == '\"' || c == '\'';
        }).base();

        result = (wsback <= wsfront ? string() : string(wsfront, wsback));

        // Remove quote characters from inside the string
        result.erase(remove(result.begin(), result.end(), '\"'), result.end());
        result.erase(remove(result.begin(), result.end(), '\''), result.end());

        return result;
    }

    EOSLIB_SERIALIZE(struct_post, (id)(network_id)(suffix)(post_action)(userid)(posted_by)(sign_type)(pub_key)(nft_id)(unsigned_data)(signed_data)(upvotes)(downvotes)(tps_posted)(tps_created)(tps_expires));
};