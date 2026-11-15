import numpy as np, matplotlib.pyplot as plt
from pathlib import Path

here = Path(__file__).parent
f = here / "solverInfo_0.dat"          # residual file
cf = here / "coefficient.dat"          # optional: forceCoeffs file, if you copied it here

lines = f.read_text().splitlines()
hdr = [l for l in lines if l.startswith("#")][-1].lstrip("# ").split()
print(hdr)                             # check the column names if nothing plots

# only read the numeric columns we need (solver-name columns are text)
cols = [0] + [i for i, n in enumerate(hdr) if n.endswith("_initial")]
data = np.genfromtxt(f, comments="#", usecols=cols)

fig, ax = plt.subplots(1, 2 if cf.exists() else 1, figsize=(12, 4), squeeze=False)
ax = ax[0]
for j, i in enumerate(cols[1:], start=1):
    ax[0].semilogy(data[:, 0], data[:, j], label=hdr[i].replace("_initial", ""))
ax[0].set_xlabel("iteration"); ax[0].set_ylabel("initial residual"); ax[0].legend()

if cf.exists():
    c = np.genfromtxt(cf, comments="#")
    ax[1].plot(c[:, 0], c[:, 2])       # column 2 = Cd; check coefficient.dat header
    ax[1].set_xlabel("iteration"); ax[1].set_ylabel("Cd")

plt.tight_layout(); plt.show()