docker run --network=network-mmo --ip=172.28.1.3 --name mmo-server-code  --ulimit core=-1 --security-opt seccomp=unconfined -v e:/mmo-server/server-code:/data/mmorpg/server-code -v e:/mmo-server/server-res:/data/mmorpg/server-res -v e:/mmo-server/share-code:/data/mmorpg/share-code -v e:/mmo-server/data/log:/data/log  --user=ubuntu mmo-server-depends