# Web server views

The Fost webserver makes use of the views defined within the fost-urlhandler library to handle all of its core features. It supports loading of plug ins that can be used

## Running the web server

The web server command line typically looks something like this:

    fost-webserver [-C directory] [-m mime-config.json] [-h binding-ip] [-p port-number] \
        configfile1.json [configfile2.json [configfile2.json ...]]

One or more configuration files typically tell it information about virtual hosts and which views to mount at which URLs (see below). By default the web server will bind to localhost only and to port 8001. The host binding can be overridden with `-h`, e.g. `-h 0` binds to all IP numbers. The port number can be changed with `-p`.

    fost-webserver -h 0 -p 5000 mysite.json

This will bind to all IP numbers and port number 5000 and load the config file `mysite.json`.

Configuration files are loaded in the order presented and the options `-m`, `-h` and `-p` override the configuration in the files. The configuration files load straight into the Fost settings database. The first two layers of objects are therefore special in that they represent individual settings.

    {
        "Section"; {
            "Setting 1": ....
            "Setting 2": ....
        }
    }

The `-C` option can be used to set a new working directory for the web server. When set this is the first action taken.

[TODO: Link to documentation on the settings database.]


## Configuration

A minimal configuration looks like this:

    {
        "webserver": {
            "hosts": {
                "": "fost.response.404"
            }
        }
    }

This tells it to answer for all hosts with a 404. The host header can be used to route to different views:

    {
        "webserver": {
            "hosts": {
                "": "fost.response.404",
                "localhost": "fost.response.501"
            }
        }
    }

Now requests to `http://localhost:8001` will respond wtih a 501 and all other requests will get a 404.


# Views

## Routing

## Middleware

## Response codes
