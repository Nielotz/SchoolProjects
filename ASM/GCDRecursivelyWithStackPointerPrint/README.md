# Calculate recursively Greatest Common Divisor with stack pointer print on every call written in Assembly as dynamic library.

## Sample usage
Input:
```
./make_shared_and_run.sh 120 5
```
Result:
```
CALL address: 7ffde310e828 deep level: 0
CALL address: 7ffde310e820 deep level: 1
RET address: 7ffde310e820 deep level: 1
RET address: 7ffde310e828 deep level: 0
Result for GCD(120, 5) = 5
```
Input:
```
./make_shared_and_run.sh 12345 54321
```
Result:
```
CALL address: 7ffe00288798 deep level: 0
CALL address: 7ffe00288790 deep level: 1
CALL address: 7ffe00288788 deep level: 2
CALL address: 7ffe00288780 deep level: 3
CALL address: 7ffe00288778 deep level: 4
CALL address: 7ffe00288770 deep level: 5
CALL address: 7ffe00288768 deep level: 6
RET address: 7ffe00288768 deep level: 6
RET address: 7ffe00288770 deep level: 5
RET address: 7ffe00288778 deep level: 4
RET address: 7ffe00288780 deep level: 3
RET address: 7ffe00288788 deep level: 2
RET address: 7ffe00288790 deep level: 1
RET address: 7ffe00288798 deep level: 0
Result for GCD(12345, 54321) = 3
```


## How to run.
On linux:<br>
- Add priviledge to run:<br>
chmod +x make_c_and_run.sh<br>
- then start<br>
./[make_shared_and_run.sh](make_shared_and_run.sh)
