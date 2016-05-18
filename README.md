# CT-PPS Run control 
Run control for the CT-PPS 2016 beam test (using the `QuickUSB` board)

### How to build
First, fetch all submodules from their respective repositories:
```bash
sh init.sh
```
If the `QuickUSB` kernel module has not yet been loaded, run (as root):
```bash
cd external/xpedaq/quickusb-2.15.2/linux/ && source setup.sh && qusb_install && cd -
```
Then, let `cmake` do its magic:
```bash
mkdir build
cd build && cmake ..
make -j [...] ppsFetch
```
The main dependencies inherited from the VME DAQ used in 2015, and which should be dropped soon (?), are:
 * `libxml2` for parsing the run XML configuration files
 * `sqlite3` for the management of the online condition database
