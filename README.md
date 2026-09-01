# Docker Labs

# Basic Steps to L2L

1. Get everyone's email in `emails.txt`, where each email is on a seperate line

2. To start all the containers, run:

```bash
$ labs-cli instantiate -u emails.txt -i ubuntu_ssh
```

The `-u` is to specify which file all the User's emails are in,

And the `-i` is to specify which docker image to use

(docker images can be found using `$ docker image ls`)


## Add an individual account

1. Get their email

2. Run:

```bash
$ labs-cli add -u email@example.com -i ubuntu_ssh
```

The `-u` specifies the email of the individual, and `-i` is for the docker image



## Remove a user's container

1. Run:

```bash
$ docker ps -a
```

This will return a list of all current containers, the names are on the right and contain the user's email


2. After getting the name of the container for the user, run:

```bash
$ labs-cli rm -n example_example_com_abc
```

In this case `-n` is to specify the name of the container



## Take down all of the containers
1. Run:

```bash
$ labs-cli nuke
```


### Common Trouble shooting

#### If all commands are throwing `nlohman::json` errors:
Check to see if CASSA's IP has changed (`$ curl ipinfo.io`), if it has:
1. Change the cloudflare permissions on the token
2. Reroll the token
3. Change line 1 of `auth.txt`
4. Run:
```bash
$ cat auth.txt > labs-cli cloudflare create_conn_str
```

This will print out a string of comma seperated values, save this value as `DOCKER_LABS_CONN_STR` in the environment/path

#### A user has f***** their container
1. Find the container name 

```bash
$ docker ps -a
```

2. Try:
```bash
$ labs-cli docker reset -n example_example_com
```

3. If that doesn't work, just remove their container and add a new one
4. If that doesn't work, make a new one under a new email
