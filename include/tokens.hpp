  //tokens.hpp
  
  public:

    ACTION sysaddtoken(const uint64_t& id, const name& contract, const string& sym_code, const uint8_t& prec);

    ACTION sysdeltoken(const uint64_t& id);

  private:

    void _sysaddtoken(const uint64_t& id, const name& contract, const string& sym_code, const uint8_t& prec);

    void _sysdeltoken(const uint64_t& id);

    void _incr_global_token_bal(const uint64_t& id, const asset& token);

    void _decr_global_token_bal(const uint64_t& id, const asset& token);

    struct_token gettoken(const uint64_t& id);

    struct_token gettoken(const name& contract, const symbol_code& sym_code);

    // scope: get_self()
    TABLE tokenstbl {
        uint64_t id;                     //id of the token
        struct_token t;                  //token data, also stores the total value held in virtual wallets

        uint64_t primary_key() const { return id; }

        // secondary key using contract.value and symbol_code
        uint128_t by_contract_and_symbol() const {
            uint128_t high = static_cast<uint128_t>(t.contract.value);
            uint128_t low = static_cast<uint128_t>(t.token.symbol.code().raw()); 
            return (high << 64) | low;
        }
    };

    typedef multi_index<name("tokens"), tokenstbl,
        indexed_by<name("contractsym"), const_mem_fun<tokenstbl, uint128_t, &tokenstbl::by_contract_and_symbol>>
    > tokens_index;

