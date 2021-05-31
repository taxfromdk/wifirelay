This project is a small fix for a pain I had when working from home. I could not reset my dev board when working from home over the network. 

I had a couple of wifi relays from aliexpress lying around. I was not too impressed with the software that came along, as it required cloud sign in. 

So I decided to reverse the sytem and see if I could reprogram the device. I could connect 3.3 volts and ground in a header and the RX and TX pins directly on the ESP8266(?). 

I spent a little time testing what IO pins were routed toward LED, Relay and button.

It turned out the button also controlled the pin that enables programming mode, so all was set.

The sketch connects to my wifi and exposes a very simple on off functionality.

Its not pretty but an example of fixing a pain with stuff lying around.