import numpy

def gather(ps, type_byte, fields):
  if isinstance(fields, str):
    fields = (fields,)
  data_list = [[m.__getattribute__(e) for e in fields]
              for m in ps.msg_byte[type_byte]]

  return numpy.array(data_list);
