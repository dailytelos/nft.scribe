//nftusers.cpp

void nftscribe::_nftuser_user_create(const name& network_id, const name& userid) {
    nftuser_index nftuser_table(get_self(), network_id.value);

    // Ensure the user does not already exist
    auto itr = nftuser_table.find(userid.value);
    check(itr == nftuser_table.end(), "User already exists in this network.");
    

    // Emplace new user
    nftuser_table.emplace(get_self(), [&](auto& user){
        user.userid = userid;
        user.u = struct_nft_user(userid);
    });
}

void nftscribe::_nftuser_token_transfer_in(name netw_id_to, name userid_to, struct_token cToken, string sMemo) {
    nftuser_index nftusers(_self, netw_id_to.value);
    auto user_itr = nftusers.find(userid_to.value);

    // Ensure user exists
    eosio::check(user_itr != nftusers.end(), "User not found.");

    // Modify user's balance
    nftusers.modify(user_itr, _self, [&](auto& user) {
        user.u.add_token_to_balance(cToken.id, cToken.token); 
    });

    // Call _nft_incr_token function
    _nft_incr_token(user_itr->u.suffix, netw_id_to, cToken); //update nft service token balance
    _incr_global_token_bal(cToken.id, cToken.token);  //update global token balance
}

void nftscribe::_nftuser_token_transfer_out(name netw_id_from, name userid_from, name to, struct_token cToken, string sMemo, struct_post cPost) {
    nftuser_index nftusers(_self, netw_id_from.value);
    auto user_itr = nftusers.find(userid_from.value);

    // Ensure user exists
    eosio::check(user_itr != nftusers.end(), "User not found.");

    // Modify user's balance
    nftusers.modify(user_itr, _self, [&](auto& user) {
        user.u.sub_token_from_balance(cToken.id, cToken.token); 
        user.u.add_trx(cPost.tps_created); // Update recent transaction
    });

    // Send transfer action to "eosio.token"
    action(
        permission_level{ _self, "active"_n },
        cToken.contract,
        "transfer"_n,
        std::make_tuple(_self, to, cToken.token, sMemo) 
    ).send();

    // Call _nft_decr_token function
    _nft_decr_token(user_itr->u.suffix, netw_id_from, cToken); 
    _decr_global_token_bal(cToken.id, cToken.token);  //update global token balance
}

void nftscribe::_nftuser_token_transfer_internal(name netw_id_from, name userid_from, name netw_id_to, name userid_to, struct_token cToken, string sMemo, struct_post cPost) {
    nftuser_index from_nftusers(_self, netw_id_from.value);
    auto from_user_itr = from_nftusers.find(userid_from.value);

    nftuser_index to_nftusers(_self, netw_id_to.value);
    auto to_user_itr = to_nftusers.find(userid_to.value);

    // Ensure both users exist
    eosio::check(from_user_itr != from_nftusers.end(), "Sender not found.");
    eosio::check(to_user_itr != to_nftusers.end(), "Receiver not found.");

    // Modify balances of both sender and receiver
    from_nftusers.modify(from_user_itr, _self, [&](auto& user) {
        user.u.sub_token_from_balance(cToken.id, cToken.token); 
        user.u.add_trx(cPost.tps_created); // Update recent transaction
    });

    to_nftusers.modify(to_user_itr, _self, [&](auto& user) {
        user.u.add_token_to_balance(cToken.id, cToken.token); 
        user.u.add_trx(cPost.tps_created); // Update recent transaction
    });

    // Call _nft_decr_token function for sender
    _nft_decr_token(from_user_itr->u.suffix, netw_id_to, cToken); 

    // Call _nft_incr_token function for receiver
    _nft_incr_token(to_user_itr->u.suffix, netw_id_to, cToken); 
}

void nftscribe::_nftuser_exe_native(name netw_id_exe, name userid_exe, struct_post::struct_exe cExe, struct_post cPost) {
    // Open the nftuser_index with the given scope
    nftuser_index users(netw_id_exe, netw_id_exe.value);

    // Find the user by its ID
    auto iter_user = users.find(userid_exe.value);
    check(iter_user != users.end(), "User not found!");

    // Check if the action exists in the contract
    check(is_account(cExe.contract), "Contract account does not exist.");

    action(
        permission_level{userid_exe, "active"_n},
        cExe.contract, 
        cExe.action,
        std::make_tuple(cExe.name_a, cExe.name_b, cExe.uint_a, cExe.uint_b, cExe.int_a, cExe.int_b, cExe.str_a, cExe.str_b, cExe.a_token)
    ).send();

    // Modify the user record to update the transactions
    users.modify(iter_user, same_payer, [&](auto& user) {
        user.u.add_trx(cPost.tps_created);  // Call the add_trx function to increment the transaction count
    });
}

// get_nft_number_from_name(name userid) -- version 10a (2023-09-30)
// a-z & 1-5 are valid sName 1=1, 5=5, a=6, y=30, z=0
uint64_t nftscribe::get_nft_number_from_name(name userid) {
    vector<string> aName = split(userid.to_string(), ".");
    check(aName.size() == 2, "Invalid userid specified for user.");

    string sName = aName[0];  //max 5 digits
    string sSuffix = aName[1]; //max 6 digits

    check((sName.size() <= 5) && (sName.size() >= 1), "get_nft_number_from_name - Username of userid is too long / too short, between 5 and 1 digits inclusive. ");
    check((sSuffix.size() <= 6) && (sSuffix.size() >= 1), "get_nft_number_from_name - Suffix of userid is too long / too short, between 6 and 1 digits inclusive. ");

    // Helper function to convert character to its corresponding value
    auto char_to_value = [](char c) -> uint64_t {
        if (c >= '1' && c <= '5') return c - '0';
        if (c >= 'a' && c <= 'y') return c - 'a' + 6;
        if (c == 'z') return 0;  // Special case for 'z'
        return 0;  // Default, shouldn't happen with checks in place
    };

    uint64_t result = 0;
    for (size_t i = 0; i < sName.size(); ++i) {
        result *= 31;  // Base 31
        result += char_to_value(sName[i]);
    }

    return result;
}

// get_nft_name_from_number(uint64_t nft_number, name suffix) -- version 10b (2023-09-30)
// a-z & 1-5 are valid sName 1=1, 5=5, a=6, y=30, z=0
name nftscribe::get_nft_name_from_number(uint64_t nft_number, name suffix) {
    
    // Ensure the suffix is valid
    check(suffix.length() >= 1 && suffix.length() <= 6, "Invalid suffix length. Must be between 1 and 6 characters.");

    // Ensure nft_number is not past maximum allowed
    check(nft_number <= 28629150, "get_nft_name_from_number received too high of a value for nft_number, max is 28629150. ");

    // Helper function to convert a value into its corresponding character
    auto value_to_char = [](uint64_t value) -> char {
        if (value >= 1 && value <= 5) return '0' + value;
        if (value >= 6 && value <= 30) return 'a' + value - 6;
        if (value == 0) return 'z';  // Special case for 0
        return ' ';  // Default, shouldn't happen with checks in place
    };

    string result_name = "";  // The string representation of the resulting name without the suffix

    do {
        uint64_t remainder = nft_number % 31;  // Get remainder of nft_number divided by 31
        result_name.insert(result_name.begin(), value_to_char(remainder));  // Convert remainder to char and prepend to result_name
        nft_number /= 31;  // Divide nft_number by 31 to get the next digit
    } while (nft_number > 0);
    
    // Now we concatenate the name with the provided suffix
    string full_name_string = result_name + "." + suffix.to_string();
    name full_name(full_name_string);

    return full_name;
}

//function only validates format, not whether user was created yet
bool nftscribe::_nftuser_is_name_valid_format(name userid, uint64_t nft_number) {
    vector <string> aName = split(userid.to_string(), ".");
    check(aName.size() == 2, "_nftuser_name_valid - Invalid userid specified for user.");

    string sName = aName[0];  //max 5 digits
    string sSuffix = aName[1];

    check((sName.size() <= 5) && (sName.size() >= 1), "Username of userid is too long / too short, between 5 and 1 digits inclusive. ");
    check((sSuffix.size() <= 6) && (sSuffix.size() >= 1), "Suffix of userid is too long / too short, between 6 and 1 digits inclusive. ");

    name name_from_nft_number = get_nft_name_from_number(nft_number, name(sSuffix));

    return name_from_nft_number.value == userid.value;
}
