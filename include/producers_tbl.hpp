//producers_tbl.hpp

//TABLE format extracted from:
//  https://github.com/telosnetwork/telos.contracts/blob/9c3756ca8660e495dd55c66e2c2f817de5f9aebb/contracts/eosio.system/include/eosio.system/eosio.system.hpp#L471


inline eosio::block_signing_authority convert_to_block_signing_authority( const eosio::public_key& producer_key ) {
      return eosio::block_signing_authority_v0{ .threshold = 1, .keys = {{producer_key, 1}} };
   }

   
// Defines `producer_info` structure to be stored in `producer_info` table, added after version 1.0
   TABLE producer_info {
      name                                                     owner;
      double                                                   total_votes = 0;
      eosio::public_key                                        producer_key; /// a packed public key object
      bool                                                     is_active = true;
      // TELOS BEGIN
      std::string                                              unreg_reason;
      // TELOS END
      std::string                                              url;
      uint32_t                                                 unpaid_blocks = 0;
      // TELOS BEGIN
      uint32_t                                                 lifetime_produced_blocks = 0;
      uint32_t                                                 missed_blocks_per_rotation = 0;
      uint32_t                                                 lifetime_missed_blocks;
      // TELOS END
      time_point                                               last_claim_time;
      uint16_t                                                 location = 0;
      // TELOS BEGIN
      uint32_t                                                 kick_reason_id = 0;
      std::string                                              kick_reason;
      uint32_t                                                 times_kicked = 0;
      uint32_t                                                 kick_penalty_hours = 0;
      block_timestamp                                          last_time_kicked;
      // TELOS END
      eosio::binary_extension<eosio::block_signing_authority>  producer_authority; // added in version 1.9.0

      uint64_t primary_key()const { return owner.value;                             }
      double   by_votes()const    { return is_active ? -total_votes : total_votes;  }
      bool     active()const      { return is_active;                               }

      // The unregprod and claimrewards actions modify unrelated fields of the producers table and under the default
      // serialization behavior they would increase the size of the serialized table if the producer_authority field
      // was not already present. This is acceptable (though not necessarily desired) because those two actions require
      // the authority of the producer who pays for the table rows.
      // However, the rmvproducer action and the onblock transaction would also modify the producer table in a similar
      // way and increasing its serialized size is not acceptable in that context.
      // So, a custom serialization is defined to handle the binary_extension producer_authority
      // field in the desired way. (Note: v1.9.0 did not have this custom serialization behavior.)

      // TELOS EDITED WITH CUSTOM FIELDS
      template<typename DataStream>
      friend DataStream& operator << ( DataStream& ds, const producer_info& t ) {
         ds << t.owner
            << t.total_votes
            << t.producer_key
            << t.is_active
            << t.unreg_reason
            << t.url
            << t.unpaid_blocks
            << t.lifetime_produced_blocks
            << t.missed_blocks_per_rotation
            << t.lifetime_missed_blocks
            << t.last_claim_time
            << t.location
            << t.kick_reason_id
            << t.kick_reason
            << t.times_kicked
            << t.kick_penalty_hours
            << t.last_time_kicked;

         if( !t.producer_authority.has_value() ) return ds;

         return ds << t.producer_authority;
      }

      // TELOS EDITED WITH CUSTOM FIELDS
      template<typename DataStream>
      friend DataStream& operator >> ( DataStream& ds, producer_info& t ) {
         return ds >> t.owner
                   >> t.total_votes
                   >> t.producer_key
                   >> t.is_active
                   >> t.unreg_reason
                   >> t.url
                   >> t.unpaid_blocks
                   >> t.lifetime_produced_blocks
                   >> t.missed_blocks_per_rotation
                   >> t.lifetime_missed_blocks
                   >> t.last_claim_time
                   >> t.location
                   >> t.kick_reason_id
                   >> t.kick_reason
                   >> t.times_kicked
                   >> t.kick_penalty_hours
                   >> t.last_time_kicked
                   >> t.producer_authority;
      }
   };


typedef eosio::multi_index< "producers"_n, producer_info,
                               indexed_by<"prototalvote"_n, const_mem_fun<producer_info, double, &producer_info::by_votes>  >
                             > producers_table;