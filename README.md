# Make your own ATM!

## Intro

We know the main principle of making your own ATM, as well as the principle of multi-threading is avoiding Memory Racing, or say Memory Conflict, which is always happened at concurrently writing.

For example, your bank account has $200. You withdraw in the same time, and the program withdraw will do:

```
withdraw:
    1. get the current balance X from server
    2. X minus withdraw amount
    3. Give costumer the money
    3. return new X to server
```

Okay. If you did 2 withdraw in the same time, it is possible that two programs all have X = 200 since they read the number at the time that the new X is not returned. Therefore, you will get 200, and your balance is still 200.

# Let's do it!

Assume 
