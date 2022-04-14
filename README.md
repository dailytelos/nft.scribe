# carbon.cert

REDESIGN
-------------

NEW PROCESS
-----------

DONE: EBC Draft--->EBC Submit-->EBC Admin Appr-->Production Draft/Submit-->Supplier Levy-->Producer Appr-->Admin Appr-->Issue Credits-->Claim Credits

**Bridge Out Send**-->Bridge Admin Appr-->BRIDGE ACTIVITY

Retire Credits Draft/Submit-->Admin Appr

Send->Resolve_Send

-----------


Use new framework.

- Update org auths to specify companies to allow for: producer, or supplier specification
- Simplify STATUS updates
- Simplify AUTHORIZED settings




TODO List
--------------
- Supplier sends invite code to supplier
- Supplier must approve C-sink
- Supplier should upload PDF of purchase invoice / receipt
- Production CERT holder gets the token issued

token ISSUES on csink
- when tokens are issued on csink, move to retirement que FIFO
- enforce 100% issue based on c-sink

- issue token
- claim token

other
- Login (PHP + Nodeos) full-cycle logins (session unique login key -> EOSIO -> PHP queries key was posted (validated))
- prevent new user creation


ISSUES
----------------



FINISHED
--------------
- switch final approval status to specific value for all things

ebc / production
- update orgid ebc certificate id reference, when ebc cert is approved by admin
- when submitted validate production cert based on EBC cert
- when submitted validate timeframe of EBC cert is still validate

csink
- when drafted validate c-sink against production cert
- when approved by admin, apply sinks against production


- Better validation of data vars passed
- (It's ok, because simplifies admin users) Can approve draft not yet submitted
- test ACTION datasubmit
- test void _datasubmit
- ACTION datasubmit
- void _datasubmit
- update _datasubmit for deletions (both types)
- test ACTION datadraft
- test ACTION _datadraft
- ACTION datadraft
- void _datadraft
- Rebuilt contract using string data (more dynamic)

