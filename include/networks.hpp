  //networks.hpp
  
  public:

    ACTION netwreg(const name& id, const string& title, const string& chain_id, const string& ticker, const string& block_expl);

    ACTION netwactive(const name& id, const uint8_t& active);
    
  private:

    void _netwreg(const name& id, const string& title, const string& chain_id, const string& ticker, const string& block_expl);

    void _netwactive(const name& id, const uint8_t& active);

    bool netw_is_active(name id);

    uint128_t get_post_count(const name& id);

    void incr_post_count(const name& id);

    // scope: get_self()
    TABLE networks {
      name id;             //network id, example: "eth.mainnet"
      string title;        //such as "Ethereum Mainnet" or "Telos EVM Mainnet", limit 128 characters
      string chain_id;     //such as : 1 (eth.main) or 40 (telos.evm), max 256 chars, respective to network identification https://chainlist.org/
      uint8_t active;      //1 = operational, 0 = disabled by contract admins, disabled has NFT registrations disabled, oracle updates disabled
      string ticker;       //network currency ticker such as "TLOS" or "ETH", max 32
      string block_expl;   //block explorer link, such as:  https://www.teloscan.io, max 256 chars
      uint128_t post_count;  //counts the number of oracle posts to the chain, and is used as an index for TABLE posts

      //Future-proofing variables, may be needed for some networks
      // or future updates to the technology
      vector<string> s_data;
      vector<uint128_t> u_data;
      vector<int64_t> n_data;
      vector<asset> a_data;

      uint64_t primary_key() const { return id.value; }
    };

    typedef multi_index<name("networks"), networks> networks_index;
  