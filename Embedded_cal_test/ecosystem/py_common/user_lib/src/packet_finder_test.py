from packet_finder import *
from numpy.random import random_integers, shuffle
from numpy import concatenate, ones, append, array_equal
import sys
from time import sleep
from sets import Set


###############################################################################
# Helper Functions

# create a random length random packet
def RandomPacket(data_min, data_max):
  msg_type = array(random_integers(0,255,1), dtype=c_uint8)
  data = array(random_integers(0,255,random_integers(data_min, data_max)), dtype=c_uint8)
  format_string = "B" * data.size
  return FormPacket(msg_type, data, format_string)

# mangle one byte of a packet
def MangleByte(packet):
  index = random_integers(0,packet.size-1)
  new_byte = random_integers(0,255)
  while new_byte == packet[index]:
    new_byte = random_integers(0,255)
  packet[index] = new_byte
  return packet

# make random length starting or ending fragments of a packet
def Fragment(packet):
  index = random_integers(1,packet.size-1)
  if 1 == random_integers(0,1):
    packet = packet[:index]
  else:
    packet = packet[index:]
  return packet
    
# compare two lists of lists
#def Compare(A, B):
#  both = 0
#  for a in A:
#    for b in B:
#      if array_equal(a,b):
#        both = both + 1
#        break
#  only_A = len(A) - both
#  only_B = len(B) - both
#  return (both, only_A, only_B)

# compare two lists of lists
#def CompareCollect(A, B):
#  both = []
#  only_A = []
#  only_B = []
#  for a in A:
#    inB = False
#    for b in B:
#      if array_equal(a,b):
#        both.append(a)
#        inB = True
#        break
#    if not inB:
#      only_A.append(a)
#  for b in B:
#    inA = False
#    for a in A:
#      if array_equal(a,b):
#        inA = True
#        break
#    if not inA:
#      only_B.append(b)
#  return (both, only_A, only_B)
  


##############################################################################
# Generate Test Data

# test parameters
data_min = 0      # min number of data bytes in packet
data_max = 64-5   # max number data bytes in packet
push_max = 64     # max number of bytes to push at once

# size of test
nGood = 10000
nMangled = 5000
nFragment = 5000

# create unique random length random good packets
good_set = Set()
while len(good_set) < nGood:
  good_set.add(tuple(RandomPacket(data_min, data_max)))
good = [array(m, dtype=c_uint8) for m in good_set]

# create random length random packets with one byte mangled
corrupted = []
for i in xrange(nMangled):
  packet = RandomPacket(data_min, data_max)
  corrupted.append(MangleByte(packet))

# create random length fragments of random good packets
fragment = []
for i in xrange(nFragment):
  packet = RandomPacket(data_min, data_max)
  fragment.append(Fragment(packet))

# shuffle packets together
test_pieces = (good + corrupted + fragment)
test_labels = concatenate([ones(nGood), zeros(nMangled), zeros(nFragment)])  # 1 iff valid
test_indices = range(len(test_labels));
shuffle(test_indices)
test_pieces = [test_pieces[i] for i in test_indices]
test_labels = [test_labels[i]>0 for i in test_indices]

# get answer key for ordered recovered data
answer_key = [test_pieces[i][2:-2] for i in xrange(len(test_pieces)) if test_labels[i]]
answer_key_set = Set([tuple(m) for m in answer_key])

test = concatenate(test_pieces)


###############################################################################
# Consume Data and Gather Packets
# aggressively keeping the buffer small

print
print "Rapid Polling Consumption Test"

pf = PacketFinder()
answers = []
index = 0
while index < test.size:
  push_size = random_integers(0, push_max)
  pf.PutBytes(test[index:index+push_size])
  new_packet = True
  while new_packet:
    packet = pf.GetPacketCopy()
    new_packet = not (0 == packet.size)
    if new_packet:
      answers.append(packet)
  index = index + push_size
  
answers_set = Set([tuple(m) for m in answers])
nCorrect = len(answers_set & answer_key_set)
nMissed = len(answer_key_set - answers_set)
nPhantom = len(answers_set - answer_key_set)

print
print "Found", len(answers), "packets."
print "  correct:", nCorrect, ",", 100.0*nCorrect/len(answer_key), "%"
print "  missed: ", nMissed, ",", 100.0*nMissed/len(answer_key), "%"
print "  phantom:", nPhantom, ",", 100.0*nPhantom/len(answer_key), "%"
print


###############################################################################
# Consume Data and Gather Packets
# repeat {push bytes until overflow, then empty packets}

print
print "Buffer Overflow Test"

pf = PacketFinder()
answers = []
overflow_count = 0
index = 0
done = False
count = 0
history = []
while not done:
  # push bytes until overflow occures (guarentee some loss of data)
  while not done:
    push_size = random_integers(0, push_max)
    put_status = pf.PutBytes(test[index:index+push_size])
    
    end_index = addressof(pf.pfc.end_data.contents) - addressof(pf.pfc.buffer)
    start_index = addressof(pf.pfc.start_data.contents) - addressof(pf.pfc.buffer)
    parse_index = pf.pfc.parse_index
    packet_count = pf.pfc.packet_indices.contents.count
    history.append((packet_count, start_index, end_index, parse_index))
    
    index = index + push_size
    done = (index-1 >= test.size)
    if put_status == -1:
      overflow_count = overflow_count + 1
      break
    if put_status == -2:
      print "CRAP!!"

  # pull packets until buffer empty again
  new_packet = True
  while new_packet:
    packet = pf.GetPacketCopy()
    
    end_index = addressof(pf.pfc.end_data.contents) - addressof(pf.pfc.buffer)
    start_index = addressof(pf.pfc.start_data.contents) - addressof(pf.pfc.buffer)
    parse_index = pf.pfc.parse_index
    packet_count = pf.pfc.packet_indices.contents.count
    status = (packet_count, start_index, end_index, parse_index)
    history.append(status)
    
    new_packet = not (0 == packet.size)
    if new_packet:
      answers.append(packet)
    

  
  count = count + 1

answers_set = Set([tuple(m) for m in answers])
nCorrect = len(answers_set & answer_key_set)
nMissed = len(answer_key_set - answers_set)
nPhantom = len(answers_set - answer_key_set)

print
print "Found", len(answers), "packets after", count, "buffer overflows."
print "  correct:", nCorrect, ",", 100.0*nCorrect/len(answer_key), "%"
print "  missed: ", nMissed, ",", 100.0*nMissed/len(answer_key), "%"
print "  phantom:", nPhantom, ",", 100.0*nPhantom/len(answer_key), "%"
print
