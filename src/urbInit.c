
cur_altsetting = intf->cur_altsetting;
endpointDesc = cur_altsetting->endpoint[0].desc;

nbPackets = 40;  // The number of isochronous packets this urb should contain			
myPacketSize = le16_to_cpu(endpointDesc.wMaxPacketSize);			
size = myPacketSize * nbPackets;
nbUrbs = 5;

for (i = 0; i < nbUrbs; ++i) {
  usb_free_urb(/** ... */); // Pour être certain
  myUrb[i] = usb_alloc_urb(/** ... */);
  if (myUrb[i] == NULL) {
    //printk(KERN_WARNING "");		
    return -ENOMEM;
  }

  myUrb[i]->transfer_buffer = usb_buffer_alloc(/** ... */);

  if (myUrb[i]->transfer_buffer == NULL) {
    //printk(KERN_WARNING "");		
    usb_free_urb(myUrb[i]);
    return -ENOMEM;
  }

  myUrb[i]->dev = /** ... */;
  myUrb[i]->context = dev;
  myUrb[i]->pipe = usb_rcvisocpipe(dev, endpointDesc.bEndpointAddress);
  myUrb[i]->transfer_flags = URB_ISO_ASAP | URB_NO_TRANSFER_DMA_MAP;
  myUrb[i]->interval = endpointDesc.bInterval;
  myUrb[i]->complete = /** ... */;
  myUrb[i]->number_of_packets = /** ... */;
  myUrb[i]->transfer_buffer_length = /** ... */;

  for (j = 0; j < nbPackets; ++j) {
    myUrb[i]->iso_frame_desc[j].offset = j * myPacketSize;
    myUrb[i]->iso_frame_desc[j].length = myPacketSize;
  }								
}

for(i = 0; i < nbUrbs; i++){
  if ((ret = usb_submit_urb(/** ... */)) < 0) {
    //printk(KERN_WARNING "");		
    return ret;
  }
}
