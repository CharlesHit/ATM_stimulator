# Make your own ATM!

Sun  1 Dec 2019 01:35:16 EST

MC 325

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

## Let's do it!

Assume we have the input file like this:

```
a1 type business d 0 w 5 t 10 transactions 20 2 overdraft Y 40
a2 type personal d 0 w 0 t 0 transactions 10 1 overdraft N
dep1 d a1 1000 d a2 3000
dep2 d a1 2000 d a2 2000
c1 d a1 100 w a2 500 t a1 a2 25 c2 w a1 2500 t a1 a2 150
...
...
c9 w a1 1000 w a2 500
c10 d a1 50 d a2 200
```


### I account
```
a1 type business d 0 w 5 t 10 transactions 20 2 overdraft Y 40

a2 type personal d 0 w 0 t 0 transactions 10 1 overdraft N
```

The above line specifies information related to bank account #1. 

- Account #1 is a business account. There is a fee of 0 to deposit funds, 5 to withdraw funds and 10 when funds are transferred into the account or out to another account.

- After the 20th transaction, there is an additional fee of 2 for every transaction. 

- This account has overdraft protection and a fee of 40 will be charged for every 500 that the account borrows to complete the transaction.

#### About overdraft

As in real world, an overdraft fee will be charged for an amount, saying 500, that the client borrows from the bank to complete the transaction. In our project there is a limit of 5000 on the overdraft. 

If the bank account does not have overdraft protection, any transaction resulting in the account balance to be less than zero should not occur. See the account #2 above

### II depositor

```
dep1 d a1 1000 d a2 3000
```

The above line specifies the operations performed by depositor #1. Depositor #1 deposits 1000 into Account #1 and then deposits 3000 into Account #2

### III customer

```
c1 d a1 100 w a2 500 t a1 a2 25
```

The above line specifies the operations performed by client #1. 

Client #1 deposits 100 into Account #1, then withdraws 500 from Account#2, and then Transfers 25 from Account #1 to Account #2

## How to do?

Firstly read from the input file. Notice, don't read line by line! You suppose to read the file and put it in an array, and **launch them by creating threads**.

Why? Because it is the real life!

Notice, since you use thread, **the exec order is non-determinant**, and the result could be different.

## Your output

```
a1 type business -300 
a2 type personal 550 
a3 type business 4500 
a4 type personal -45
```

Print it on screen, or redirect to folder, either is okay.

## Appendix A More Illustraion

### some details

- When c1 made a transfer from a1 to a2 and if there is transfer fee associated to a1 and a2 then transfer fees are applied to both a1 and a2.
- The initial depositors transactions count towards the total number of transactions.

### More Overdraft!

 If the account balance after completing a particular transaction is between -1 to -500, apply

- 1 x Overdraft Fee

- -501 to -1000 apply 2 x Overdraft Fee

- -1001 to -1500, apply 3 x Overdraft Fee

- -4501 to -5000, apply 10 x Overdraft Fee 

After applying the overdraft fee, you need to check the account balance again to make sure that the transaction is valid (greater than or equal to -5000) or that you do not need to apply any more fees. 

The overdraft fee is applied once for every multiple of 500. 

So, in test_2.txt,

```
a1 type personal d 0 w 0 t 0 transactions 20 2 overdraft Y 40
dep1 d a1 1
c1 w a1 200
c2 w a1 100
c3 w a1 161
c4 w a1 490
``` 

Account 1 has overdraft protection, 0 fee to withdraw from the account, and has an account balance of 1 after the depositor deposits 1. The overdraft fee is 40.

- Client 1 withdraws 200 from account 1. 1 -200 = -199. Since -199 is less than 0 and greater than or equal to -500, the overdraft fee is applied. Account balance = -199 - 40 = -239.

- Client 2 withdraws 100 from account 1, the account balance is -239-100 = -339. Since -339 is greater than or equal to -500 and the overdraft fee has already been applied by client 1, the overdraft fee is not applied here.

- Client 3 withdraws 161 from account 1, the balance is -339-161 = -500. Since the balance is equal to -500 and the overdraft fee has already been applied by client 1, the overdraft fee is not applied here.

- Client 4 withdraws 490. -500 - 490 = -990. Since -990 is less than -500 and greater than or equal to -1000, the 40 overdraft fee is applied. -990 - 40 = -1030. However, after applying the overdraft fee, the value is less than -1000 and greater than or equal to -1500. Therefore, the overdraft fee would be applied again. The account balance would finally be -1070.

## Appendix C Basic Thread Operation in C

`Pthread`
