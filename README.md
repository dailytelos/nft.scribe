# Work in Process code!
- Certainly there are bugs in this code, not ready for production yet.

# NFT Virtual Accounts on Telos Native (tNFT)

**nft.scribe** is the Telos native oracle contract, relaying data from other blockchains such as Ethereum,
permitting users to login using their Ethereum NFT's, and perform actions on Telos Native.

This oracle relay creates a framework for gas-free Virtual Accounts on Telos Native, which is great for the operation of gas-free governance models on Telos Native by projects.  It is a great selling-point for existing Ethereum NFT projects, to transition their governance / staking to Telos Native.

## KEY FEATURES
- Unlocks the power of Telos Native for Ethereum, BSC, and other EVM chains
- Contract Owners can register their NFT project officially
- Others can register their individual NFT to participate
- Registered NFT's receive a Telos Native Virtual Account (tNFT), secured by multi-sig oracle network
- Invalid transactions are publicly verifiable as invalid, contract code in the future could even be upgraded to enforce only a valid signature is posted
-------------
## PROCESS
1. NFT Owner initiates a SignTypedDataV4 with Oracle API Server: https://docs.metamask.io/guide/signing-data.html#signtypeddata-v4
2. Oracle verifies they in fact own the NFT via public contract access
3. Oracle posts to Telos Native, their public key, their signed data, and their unsigned data
4. Oracles all vote on the transaction, if they likewise agree the data was signed by that key, and that this key in fact owns the NFT
5. Once a configured multi-sig threshold is reached at a 70% threshold, such as 5 out of 7 oracles agree, the data of the transaction is read by the nft.scribe contract and executed according to the data provided
6. nft.scribe interfaces into other Telos Native contracts where the actions are specifically approved by the contract
-------------

## tNFT Virtual Account Interoperability
- tNFT accounts use the "nft" permission from the nft.scribe contract (nft.scribe@nft)
- The nft.scribe@nft permission will call other actions on Telos Native, and this will be secure because the call will fail unless both linkauth is configured, and the ACTION is setup to deal with tNFT accounts
- A project can setup its own interfaces for linkauth, so long as the action name begins with "nft"
- Oracles can multi-sig add other actions that do not begin with "nft", but they'll likewise hold the permission and function definition limitations of tNFT accounts
- System calls "eosio" and similar, will not be accessable to virtual accounts
- "eosio.token" and the transfer command will be accessable to virtual accounts under their virtual wallet

-------------
## PROJECT REGISTRATION TIERS
-------------
- Project registration, collects 

## TODO
-------------
- Oracle Posting
- Oracle Rotation of Account @active permissions
- Execution of Approved Posting
- Virtual Accounts
