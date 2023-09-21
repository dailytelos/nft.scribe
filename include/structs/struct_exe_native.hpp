//struct_exe_native.hpp

// Structure is used to store / pass data necessary for executing actions using .send() to other contracts on Telos native
struct struct_exe_native {
    name contract;
    name action;

    name name_a;
    name name_b;

    uint64_t uint_a;
    uint64_t uint_b;

    int64_t int_a;
    int64_t int_b;

    string str_a;
    string str_b;

    asset a_token;

    EOSLIB_SERIALIZE(struct_exe_native, (contract)(action)(name_a)(name_b)(uint_a)(uint_b)(int_a)(int_b)(str_a)(str_b)(a_token));
};