You'll need these for building MRuby & CEF
```
sudo apt-get install ruby rake git cmake libgtkglextmm-x11-1.2-dev libudev1 clang bison
```

You may not need to do this. 
See: http://askubuntu.com/questions/369310/how-to-fix-missing-libudev-so-0-for-chrome-to-start-again
```
sudo ln -s /lib/x86_64-linux-gnu/libudev.so.1 /lib/x86_64-linux-gnu/libudev.so.0
```

Rubium uses the mruby-apr gem, which depends on APR, so we'll build that.
(It doesn't matter where you do the building, as long as you let make install to the default dir)
```
mkdir apr
cd apr
wget http://apache.arvixe.com//apr/apr-1.5.2.tar.gz
tar xfz apr-1.5.2.tar.gz
cd apr-1.5.2
./configure
make
sudo make install
```

Now to clone rubium
```
git clone http://github.com/jbreeden/rubium
cd ./rubium
```

Rubium uses a specific version of mruby. (Same as upstream, with a custom build_config.rb file)
```
git submodule init
git submodule update
```

_Manual Step!_
- CEF doesn't distribute to robots. 
- I should probably mirror the correct version (if allowed) so the download can be scripted.
Download CEF's binary distribution and extract it into the rubium folder.
(Rubium will glob for cef_binary* and use the last one it finds, which should be the highest version you have made available.)

Now build the cefclient
```
rake cef:cefclient
```

Then build mruby
_If you're building on 32-bit linux, and get complaints about off64_t being undefined,
change this to `rake mruby:build CFLAGS="-D_LARGEFILE64_SOURCE"`_
```
rake mruby:build
```

Finally, package rubium
```
rake package
````

Rubium should end up in a new folder (also called rubium). Just put this on your path. Then you can try running `rubium` in some of the provided `samples/`.
