//nftusers.hpp

public:

    //No public ACTION available because the oracles must perform the ACTION post to interact on behalf of the users

private:

    void _nftuser_user_create(const name& network_id, const name& userid);

    void _nftuser_token_transfer_in(name netw_id_to, name userid_to, struct_token cToken, string sMemo);

    void _nftuser_token_transfer_out(name netw_id_from, name userid_from, name to, struct_token cToken, string sMemo, struct_post cPost);

    void _nftuser_token_transfer_internal(name netw_id_from, name userid_from, name netw_id_to, name userid_to, struct_token cToken, string sMemo, struct_post cPost);

    void _nftuser_exe_native(name netw_id_exe, name userid_exe, struct_post::struct_exe cExe, struct_post cPost);

    uint64_t get_nft_number_from_name(name userid);

    name get_nft_name_from_number(uint64_t nft_number, name suffix);

    bool _nftuser_is_name_valid_format(name userid, uint64_t nft_number);

    // scope: TABLE networks->id
    TABLE nftusers {
        name userid;          // Username prefix may be 4 characters, suffixes may be up to 7 characters for a project.  Example: name("12345.suffix");
        struct_nft_user u;

        uint64_t primary_key() const { return userid.value; }
    };

    typedef multi_index<name("nft.users"), nftusers> nftuser_index;
