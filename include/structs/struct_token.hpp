//struct_token.hpp

// a vector of these stores the tokens registered for a given nftservice
struct struct_token {
    uint64_t id;           //internal reference to the tokens TABLE, including the native contract name
    name contract;         //name of contract on Telos Native that holds the token balances / transfer actions
    asset token;           //symbol(symbol_code, precision)

    struct_token() {
        id          = 0;
        contract    = name(NAME_NULL);
        token       = asset(0, symbol(symbol_code("NULL"), 4));
    };

    struct_token(uint64_t i_id, name i_contract, asset i_token) {
        id          = i_id;
        contract    = i_contract;
        token       = i_token;
    };

    struct_token(uint64_t i_id, name i_contract, int64_t i_amount, string i_sym_code, uint8_t i_prec) {
        id          = i_id;
        contract    = i_contract;
        token       = asset(i_amount ,symbol(symbol_code(i_sym_code), i_prec));
    };

    EOSLIB_SERIALIZE(struct_token, (id)(contract)(token));
};