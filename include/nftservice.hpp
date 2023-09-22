  //nftservice.hpp

  public:

    ACTION nftregister(const name& auth, const name& suffix, const name& network_id, const string& nftcontract, const vector <string>& contracts, const name& admin, const string& evm_owner, const string& website,  const string& admin_email);

    ACTION nftactive(const name& auth, const name& suffix, const name& network_id, const uint8_t& active);

    ACTION nftaddtoken(const name& auth, const name& suffix, const name& network_id, const uint64_t& token_id);

    ACTION nftdeltoken(const name& auth, const name& suffix, const name& network_id, const uint64_t& token_id);

  private:

    void _nftregister(const name& auth, const name& suffix, const name& network_id, const string& nftcontract, const vector <string>& contracts, const name& admin, const string& evm_owner, const string& website,  const string& admin_email);

    void _nftactive(const name& auth, const name& suffix, const name& network_id, const uint8_t& active);

    void _nftaddtoken(const name& auth, const name& suffix, const name& network_id, const uint64_t& token_id);

    void _nftdeltoken(const name& auth, const name& suffix, const name& network_id, const uint64_t& token_id);

    void _nft_incr_token(const name& suffix, const name& network_id, const struct_token& cToken);

    void _nft_decr_token(const name& suffix, const name& network_id, const struct_token& cToken);

    //Virtual Accounts
    // ... will execute against ACTION virtualexe(...) using nftexe permission
    //
    // New Account Creation would Cost: (Aug 29, 2023)
    // 2996 bytes of RAM is worth 0.1130 TLOS at current market price
    // 0.009 CPU
    // 0.001 NET
    // -- Total: 0.123 TLOS per new account --
    // So by doing virtual account it allows for savings
    
    // scope: TABLE networks->id
    TABLE nftservice {
      name suffix;                    //id of network, also the virtual account suffix, this is used a scope in the accounts table (max 6 characters)
      string nftcontract;             //string up to 128 characters, for ERC721/etc. address of NFT contract, value can never be changed after registration
      vector <string> contracts;      //These are associated contracts, can be vault standard ERC4626/etc.
                                      //if a user is found to have a valid NFT at any of these contracts, they will have account access
                                      //public oracles must be configured to find the users inside these other contracts
                                      //associated contracts can be updated by the admin

      uint8_t active;       //1 = operational, 0 = disabled by contract admins, disabled has NFT registrations disabled, oracle updates disabled
      uint64_t official;    //0 = unverified / unofficial account, # = block number where verification was last posted on Telos Native
      uint64_t updates;     //counts the # of individual oracle user updates recorded under this NFT Service

      name admin;           //the Telos Native administrator account
      string evm_owner;     //evm chain owner

      string website;       //website of project, up to 256 characters
      string admin_email;   //administrator email, up to 64 characters

      vector <struct_token> tokens;    //list of tokens and balances contained in this specific nftservice

      //Future-proofing variables, may be needed for some networks
      // or future updates to the technology
      vector<string> s_data;
      vector<uint128_t> u_data;
      vector<int64_t> n_data;
      vector<asset> a_data;

      uint64_t primary_key() const { return suffix.value; }
    };

    typedef multi_index<name("nftservices"), nftservice> nftsrv_index;

