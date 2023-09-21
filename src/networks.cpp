//networks.cpp

ACTION nftscribe::netwreg(const name& id, const string& title, const string& chain_id, const string& ticker, const string& block_expl) {
    require_auth(get_self());

    check(title.size() <= 128, "title is too large, limit of 128 characters. ");
    check(chain_id.size() <= 256, "chain_id is too large, limit of 256 characters. ");
    check(ticker.size() <= 32, "ticker is too large, limit of 32 characters. ");
    check(block_expl.size() <= 256, "block_expl is too large, limit of 256 characters. ");

    _netwreg(id, title, chain_id, ticker, block_expl);
}

ACTION nftscribe::netwactive(const name& id, const uint8_t& active) {
    require_auth(get_self());

    check((active == 0) || (active == 1), "active must be set to 0 or 1, other values are invalid.");

    _netwactive(id, active);
}

ACTION nftscribe::netwthresh(const name& id, const uint16_t& threshold) {
    require_auth(get_self());

    _netwthresh(id, threshold);
}

void nftscribe::_netwreg(const name& id, const string& title, const string& chain_id, const string& ticker, const string& block_expl) {
    
    name scope = get_self(); //contract is scope in this case
    networks_index _networks_table( get_self(), scope.value );
    auto networks_itr = _networks_table.find(id.value);

    //new network registered
    if(networks_itr == _networks_table.end()){
        _networks_table.emplace( get_self(), [&]( auto& row ) {
            row.id = id;
            row.title = title;
            row.chain_id = chain_id;
            row.active = 0;
            row.ticker = ticker;
            row.block_expl = block_expl;
            row.post_count = 0;
        });

    } else { //update existing network
        _networks_table.modify( networks_itr, get_self(), [&]( auto& row ) {
            row.title = title;
            row.chain_id = chain_id;
            row.ticker = ticker;
            row.block_expl = block_expl;
        });
    }
}

void nftscribe::_netwactive(const name& id, const uint8_t& active) {
    
    name scope = get_self(); //contract is scope in this case
    networks_index _networks_table( get_self(), scope.value );
    auto networks_itr = _networks_table.find(id.value);

    if(networks_itr == _networks_table.end()){
        check(false, "id does not exist, unable to update network to active.");

    } else { //update existing network
        _networks_table.modify( networks_itr, get_self(), [&]( auto& row ) {
            row.active = active;
        });
    }
}


void nftscribe::_netwthresh(const name& id, const uint16_t& threshold) {
    networks_index _networks_table(get_self(), get_self().value);
    auto network_itr = _networks_table.find(id.value);

    eosio::check(network_itr != _networks_table.end(), "Network not found.");
    eosio::check(threshold > 0, "Threshold must be greater than zero.");

    _networks_table.modify(network_itr, get_self(), [&](auto& network) {
        network.threshold = threshold;
    });
}

bool nftscribe::netw_is_active(name id) {
    
    name scope = get_self(); //contract is scope in this case
    networks_index _networks_table( get_self(), scope.value );
    auto networks_itr = _networks_table.find(id.value);

    if(networks_itr != _networks_table.end()){
       return (networks_itr->active == 1);
    }

    return false;
}

uint16_t nftscribe::get_threshold(const name& id) {
    networks_index _networks_table(get_self(), get_self().value);
    auto network_itr = _networks_table.find(id.value);

    eosio::check(network_itr != _networks_table.end(), "Network not found.");

    return network_itr->threshold;
}

uint128_t nftscribe::get_post_count(const name& id) {
    name scope = get_self(); //contract is scope in this case
    networks_index _networks_table( get_self(), scope.value );
    auto networks_itr = _networks_table.find(id.value);

    check(networks_itr != _networks_table.end(), "Network ID not found.");

    return networks_itr->post_count;
}

void nftscribe::incr_post_count(const name& id) {
    name scope = get_self(); // contract is scope in this case
    networks_index _networks_table( get_self(), scope.value );
    auto networks_itr = _networks_table.find(id.value);

    check(networks_itr != _networks_table.end(), "Network ID not found.");

    _networks_table.modify(networks_itr, get_self(), [&]( auto& row ) {
        row.post_count += 1;
    });
}


