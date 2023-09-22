//posts.hpp

  public:

  ACTION post(
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
);

  ACTION upvote(const name& oracle_id, const name& network_id, const uint64_t& posts_id);

  ACTION downvote(const name& oracle_id, const name& network_id, const uint64_t& posts_id);
  
  private:

  void _post(
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
);

  void _upvote(const name& oracle_id, const name& network_id, const uint64_t& posts_id);

  void _downvote(const name& oracle_id, const name& network_id, const uint64_t& posts_id);

  bool has_post_met_threshold(const name& network_id, const uint64_t& posts_id);

    // scope: TABLE networks->id
    //  per default: "eth.mainnet" will be the default network determining contract permissions
    TABLE posts {
        uint64_t id;                
        struct_post post;

        uint64_t primary_key() const { return id; }
    };

    typedef multi_index<name("posts"), posts> post_index;
