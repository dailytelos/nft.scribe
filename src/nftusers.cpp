//nftusers.cpp

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

