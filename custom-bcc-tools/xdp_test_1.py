from bcc import BPF
from bcc.utils import printb
import sys


def usage():
    print(f"""
        $ {sys.argv[0]} <network_interface>
          """)
    exit(1)


if len(sys.argv) < 2:
    usage()

device = sys.argv[1]

flags = 0
# offload_device = None

# ret = "XDP_DROP"
ret = "XDP_PASS"
ctxtype = "xdp_md"
maptype = "percpu_array"

b = BPF(text=open("xdp_test_1.c").read(),
        cflags=["-w",
                "-DRETURNCODE=%s" % ret,
                "-DCTXTYPE=%s" % ctxtype, "-DMAPTYPE=\"%s\"" % maptype])

fn = b.load_func("xdp_prog1", BPF.XDP)
b.attach_xdp(device, fn, flags)

while True:
    try:
        (task, pid, cpu, flags, ts, msg) = b.trace_fields()
        # (tag) = msg.split(b" ")
    except ValueError:
        continue
    except KeyboardInterrupt:
        exit()

    print(task)
    print(pid)
    printb(msg)

# dropcnt = b.get_table("dropcnt")
# prev = [0] * 256
# print("Printing drops per IP protocol-number, hit CTRL+C to stop")
# while 1:
#     try:
#         for k in dropcnt.keys():
#             if maptype == "array":
#                 val = dropcnt[k].value
#             else:
#                 val = dropcnt.sum(k).value
#             i = k.value
#             if val:
#                 delta = val - prev[i]
#                 prev[i] = val
#                 print("{}: {} pkt/s".format(i, delta))
#         time.sleep(1)
#     except KeyboardInterrupt:
#         print("Removing filter from device")
#         break

b.remove_xdp(device, flags)
