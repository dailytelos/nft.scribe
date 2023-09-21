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
    const string& eth_pub_key,
    const uint128_t& nft_id,
    const string& unsigned_data,
    const string& signed_data,
    const time_point_sec& tps_posted,
    const time_point_sec& tps_created,
    const time_point_sec& tps_expires
) {
    // Ensure that the action is authorized
    require_auth(get_self());

    // Call the private _post function
    _post(
        oracle_id,
        network_id,
        suffix,
        contract,
        post_action,
        userid,
        sign_type,
        eth_pub_key,
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
    const string& eth_pub_key,
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
            eth_pub_key,
            nft_id,
            unsigned_data,
            signed_data,
            Upvotes,            
            Downvotes,          
            tps_posted,
            tps_created,
            tps_expires
        );
    });

    // Increment the global ID variable
    incr_post_count(network_id);
}


ACTION nftscribe::upvote(const name& oracle_id, const name& network_id, const uint64_t& posts_id)  {
    require_auth(oracle_id);

    _upvote(oracle_id, network_id, posts_id);
}

void nftscribe::_upvote(const name& oracle_id, const name& network_id, const uint64_t& posts_id) {
    post_index posts(_self, network_id.value); // assuming _self refers to the contract's account name

    auto itr = posts.find(posts_id);
    check(itr != posts.end(), "Post not found.");

    posts.modify(itr, _self, [&](auto& p) {
        p.post.upvote(oracle_id);
    });
}

ACTION nftscribe::downvote(const name& oracle_id, const name& network_id, const uint64_t& posts_id)  {
    require_auth(oracle_id);

    _downvote(oracle_id, network_id, posts_id);
}

void nftscribe::_downvote(const name& oracle_id, const name& network_id, const uint64_t& posts_id) {
    post_index posts(_self, network_id.value); // assuming _self refers to the contract's account name

    auto itr = posts.find(posts_id);
    check(itr != posts.end(), "Post not found.");

    posts.modify(itr, _self, [&](auto& p) {
        p.post.downvote(oracle_id);
    });
}

