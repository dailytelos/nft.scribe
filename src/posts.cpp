//posts.cpp

// Public action to post data
ACTION nftscribe::post(
    const name& oracle_id,
    const name& network_id,
    const name& suffix,
    const name& contract,
    const name& post_action,
    const name& userid,
    const uint16_t& sign_type,
    const string& evm_pub_key,
    const uint128_t& nft_id,
    const string& unsigned_data,
    const string& signed_data,
    const time_point_sec& tps_posted,
    const time_point_sec& tps_created,
    const time_point_sec& tps_expires
) {
    checkfreeze();

    // Ensure that the action is authorized, that they are a registered oracle on the network
    struct_oracle cOracle = get_oracle(oracle_id, network_id);
    check(cOracle.is_active(), "Oracle status is not set to active, unable to post. ");

    // Call the private _post function
    _post(
        oracle_id,
        network_id,
        suffix,
        contract,
        post_action,
        userid,
        sign_type,
        evm_pub_key,
        nft_id,
        unsigned_data,
        signed_data,
        tps_posted,
        tps_created,
        tps_expires
    );
}

// Private function to insert a new row into the "posts" table
void nftscribe::_post(
    const name& oracle_id,
    const name& network_id,
    const name& suffix,
    const name& contract,
    const name& post_action,
    const name& userid,
    const uint16_t& sign_type,
    const string& evm_pub_key,
    const uint128_t& nft_id,
    const string& unsigned_data,
    const string& signed_data,
    const time_point_sec& tps_posted,
    const time_point_sec& tps_created,
    const time_point_sec& tps_expires
) {
    // Access the "posts" table
    post_index posts(get_self(), network_id.value);

    // Get the next available ID from the global variable
    uint64_t next_id = get_post_count(network_id);

    vector <name> Upvotes;
    Upvotes.push_back(oracle_id);

    vector <name> Downvotes;

    // Create a new row in the table
    posts.emplace(get_self(), [&](auto& row) {
        row.id = next_id;
        row.post = struct_post(
            next_id,            // Use the next available ID
            network_id,
            suffix,             //suffix
            contract,           //contract target, for internal it is "nft.scribe", for TLOS token transfer it is "eosio.token", for other contracts it's that contract
            post_action,        //post_action
            userid,             //userid of poster from nftusers.hpp
            oracle_id,          //posted by
            sign_type,
            evm_pub_key,
            nft_id,
            unsigned_data,
            signed_data,
            Upvotes,            
            Downvotes,          
            tps_posted,
            tps_created,
            tps_expires
        );

        row.post.verify();
    });

    // Increment the global ID variable
    incr_post_count(network_id);
}


ACTION nftscribe::upvote(const name& oracle_id, const name& network_id, const uint64_t& posts_id)  {
    require_auth(oracle_id);

    checkfreeze();

    _upvote(oracle_id, network_id, posts_id);
    _orcrefresh(oracle_id, oracle_id, network_id);
}

void nftscribe::_upvote(const name& oracle_id, const name& network_id, const uint64_t& posts_id) {

    post_index posts(_self, network_id.value); 

    auto itr = posts.find(posts_id);
    check(itr != posts.end(), "Post not found.");

    //if expired, then just delete, cannot be upvoted or executed
    if (current_time_point().sec_since_epoch() > itr->post.tps_expires.sec_since_epoch()) {
        posts.erase(itr); 
        return;
    }

    struct_post cPost;
    posts.modify(itr, _self, [&](auto& p) {
        p.post.upvote(oracle_id);
        p.post.verify();
        cPost = p.post;
    });


    // Call exe_out and get the returned struct_exe
    struct_post::struct_exe execution_data = cPost.exe_out();

    //validates userid matches nft_id number
    check(_nftuser_is_name_valid_format(execution_data.name_a, cPost.nft_id), "Name provided " + execution_data.name_a.to_string() + " is invalid for #(" + std::to_string(static_cast<long long>(cPost.nft_id)) + ").");

    struct_token cToken;

    if (has_post_met_threshold(network_id, posts_id)) {
        switch(execution_data.action.value) { 
            case name("new.user").value:
                //exe_data: "1.nft|name.null...a|0|0|0|0|||0.0000 NULL"
                _nftuser_user_create(network_id, execution_data.name_a);
            break;
            case name("transfer").value:
                
                cToken = gettoken(cPost.contract, execution_data.a_token.symbol.code());
                _nftuser_token_transfer_out(network_id, cPost.userid, execution_data.name_a, cToken, "Transfer Out Memo", cPost);
            break;
            case name("transfer.in").value:
                
                cToken = gettoken(cPost.contract, execution_data.a_token.symbol.code());
                _nftuser_token_transfer_internal(network_id, cPost.userid, execution_data.name_b, execution_data.name_a, cToken, "Transfer In Memo", cPost);
            break;
            default:  //execute nothing, invalid action, but execution will also prevent it from being stuck in system
            break;
        }

        posts.erase(itr); 

    } 
}

ACTION nftscribe::downvote(const name& oracle_id, const name& network_id, const uint64_t& posts_id)  {
    require_auth(oracle_id);

    checkfreeze();

    _downvote(oracle_id, network_id, posts_id);
    _orcrefresh(oracle_id, oracle_id, network_id);
}

void nftscribe::_downvote(const name& oracle_id, const name& network_id, const uint64_t& posts_id) {
    post_index posts(_self, network_id.value); // assuming _self refers to the contract's account name

    auto itr = posts.find(posts_id);
    check(itr != posts.end(), "Post not found.");

    //if expired, then just delete, cannot be upvoted or executed
    if (current_time_point().sec_since_epoch() > itr->post.tps_expires.sec_since_epoch()) {
        posts.erase(itr);
        return;
    }

    posts.modify(itr, _self, [&](auto& p) {
        p.post.downvote(oracle_id);
        p.post.verify();
    });
}

bool nftscribe::has_post_met_threshold(const name& network_id, const uint64_t& posts_id) {
    
    networks_index _networks_table(get_self(), get_self().value);
    auto network_itr = _networks_table.find(network_id.value);

    eosio::check(network_itr != _networks_table.end(), "Network not found.");
    
    uint16_t threshold = network_itr->threshold;

    post_index _posts_table(get_self(), network_id.value);
    auto post_itr = _posts_table.find(posts_id);

    eosio::check(post_itr != _posts_table.end(), "Post not found.");

    size_t num_upvotes = post_itr->post.upvotes.size();
    size_t total_votes = num_upvotes + post_itr->post.downvotes.size();

    // Ensure upvotes to total votes ratio is >= ORACLE_APPROVAL_RATIO
    bool meets_ratio = (static_cast<double>(num_upvotes) / static_cast<double>(total_votes)) >= ORACLE_APPROVAL_RATIO;
    
    // Check if the post upvotes have met or exceeded the threshold and meets the ratio.
    return (num_upvotes >= threshold) && meets_ratio; 
}

