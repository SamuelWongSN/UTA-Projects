#!/usr/bin/python
# -*- coding: utf-8 -*-
import argparse
import parser

class LTRecord(object):
    '''
    This class is the record of Lock Table. Each record in Lock Table is LTRecord.
    Lock Table is a dict, key:item, value: item's lock state

    Attribute:
        lock: string type of lock {unlock, read, write}
        no_of_read: int number of read locks
        locking_txn: list which transaction own the lock of this item
    '''
    def __init__(self, lock, txn_id):
        '''
        create a record of item.
        :param lock: create a lock for item. Two possible value: read or write
        :param txn_id: which transaction create this record
        '''
        self.lock = lock
        self.no_of_read = 0
        if lock == 'read':
            self.no_of_read += 1
        self.locking_txn = []
        self.locking_txn.append(txn_id)

    def __str__(self):
        disc = 'lock_state: ' + self.lock + ', ' + 'transaction_id: ' + self.locking_txn.__str__()
        return disc

    def add_read_lock(self, txn_id):
        self.lock = 'read'
        self.no_of_read += 1
        self.locking_txn.append(txn_id)

    def add_write_lock(self, txn_id):
        self.lock = 'write'
        self.locking_txn.append(txn_id)

    def upgrade_lock(self):
        self.lock = 'write'
        self.no_of_read = 0

    def downgrade_lock(self):
        self.lock = 'read'
        self.no_of_read = 1

    def unlock_read(self, txn_id):
        self.no_of_read -= 1
        if self.no_of_read == 0:
            self.lock = 'unlock'
        self.locking_txn.remove(txn_id)

    def unlock_write(self):
        self.lock = 'unlock'
        self.locking_txn.clear()


class TxnRecord(object):
    '''
    This class is the record of Transaction Table. Each record in Transaction Table is TxnRecord.
    Transaction Table is a dict, key:transaction_id, value: transaction's state(TxnRecord)

    Attribute:
        timestamp: int/ used to judge the orders of transactions
        state: string/ {Active, Blocked, Abort, Commited}
        lock_item: list/ store the items which lock be this transaction held
    '''
    def __init__(self, timestamp, state):
        self.timestamp = timestamp
        self.state = state
        self.lock_item = []

    def __str__(self):
        description = 'timestamp:' + str(self.timestamp) + ' statement:' + self.state
        return description


class Operation(object):
    '''
    This class is the operation list. Each operation in queue is Operation.

    Attribute:
        action: string/ the action of operation {begin, end, read, write}
        txn_id: string/ transaction of issuing this operation
        item: string/ object of this operation
    '''
    def __init__(self, txn_id, action, item=None):
        self.action = action
        self.txn_id = txn_id
        self.item = item

    def __str__(self):
        description = self.action + self.txn_id
        if self.item:
            description += '(' + self.item + ')'
        return description


class Scheduler(object):
    '''
    This class is the scheduler to address operations.

    Attribute:
        operations: list/ store the operations which need to run
        blocked_operations: list/ store the blocked operations
        lock_table: dict/ store the item state {key: item, value：LTRecord}
        transaction_table: dict/ store the transaction state {key: transaction_id, value: TxnRecord}
        executed_operation: list/ store the executed operations
        timestamp: int/ store the order of transactions
    '''
    def __init__(self, operations):
        self.operations = []
        self.blocked_operations = []
        self.lock_table = {}
        self.transaction_table = {}
        self.executed_operations = []
        self.load_operations(operations)
        self.timestamp = 1

    def load_operations(self, operations):
        '''
        This funtion load the operations from txt file. Each line has a single transaction operation.
        :param operations: string/ file path
        :return: None
        '''
        translate = {'b': 'begin',
                     'e': 'end',
                     'r': 'read',
                     'w': 'write'}
        with open(operations) as file:
            line = file.readline()
            while line:
                action = translate[line[0]]
                txn_id = line[1]
                item = None if line[2] == ';' else line[3]
                self.operations.append(Operation(txn_id, action, item))
                line = file.readline()
                line = ''.join(line.split())

    def run_operations(self):
        '''
        To Address the list of operations
        :return: None
        '''
        print('---------Operation Statement---------')
        while self.operations:
            self.run_operation(self.operations.pop(0))
            # if final operation is aborted, blocked operations need to back to operations list
            if self.blocked_operations and len(self.operations) == 0:
                for operation in self.blocked_operations:
                    self.transaction_table[operation.txn_id].state = 'Active'
                    self.operations.append(operation)
                self.blocked_operations.clear()

    def do_operation(self, operation, flag=None):
        '''
        print the log to represent the running result of each operation
        :param operation: the operation which need to log
        :param flag: the type of result.
        :return: None
        '''
        if flag == 1:
            print(operation, 'successful')
        elif flag == 2:
            print(operation, 'Abort')
        elif flag == 3:
            print(operation, 'Blocked')
        elif operation.action == 'end':
            print(operation, 'commit')
        else:
            print(operation)
        print('---Transaction Table---')
        for key, item in self.transaction_table.items():
            print(key, item)
        print('---Lock Table---')
        for key, item in self.lock_table.items():
            print(key, item)
        print('---Blocked Operations---')
        for opera in self.blocked_operations:
            print(opera)

        print()

    def run_operation(self, operation):
        '''
        run each operations
        :param operation: operation which need to be run
        :return: None
        '''
        if operation.action == 'begin':
            # create new transaction state
            self.transaction_table[operation.txn_id] = TxnRecord(self.timestamp, 'Active')
            self.do_operation(operation)
            self.timestamp += 1

        elif operation.action == 'end':
            if self.transaction_table[operation.txn_id].state == 'Abort':
                self.do_operation(operation, 2)
                return
            elif self.transaction_table[operation.txn_id].state == 'Blocked':
                self.blocked_operations.append(operation)
                return
            else:
                self.unlock(operation)
                self.do_operation(operation)
                self.transaction_table[operation.txn_id].state = 'Commited'

        else:
            if self.transaction_table[operation.txn_id].state == 'Abort':
                self.do_operation(operation, 2)
                return
            elif self.transaction_table[operation.txn_id].state == 'Blocked':
                self.blocked_operations.append(operation)
                return
            if operation.action == 'read':
                flag = self.read_lock(operation)
                if flag == 1:
                    self.executed_operations.append(operation)
                    self.transaction_table[operation.txn_id].lock_item.append(operation.item)
                self.do_operation(operation, flag)

            elif operation.action == 'write':
                flag = self.write_lock(operation)
                if flag == 1:
                    self.executed_operations.append(operation)
                    self.transaction_table[operation.txn_id].lock_item.append(operation.item)
                self.do_operation(operation, flag)

    def read_lock(self, operation):
        '''
        read_lock operation
        :param operation: the operation want to take a read lock
        :return: flag of lock result
        '''
        if operation.item not in self.lock_table.keys():
            self.lock_table[operation.item] = LTRecord('read', operation.txn_id)
            return 1
        elif self.lock_table[operation.item].lock in ['read', 'unlock']:
            self.lock_table[operation.item].add_read_lock(operation.txn_id)
            return 1
        else:
            # if lock type is write, locking_txn must have one txn.
            locked_txn_id = self.lock_table[operation.item].locking_txn[0]

            # Downgrade Operation
            if locked_txn_id == operation.txn_id:
                self.lock_table[operation.item].downgrade_lock()
                return 1
            else:
                # Cautious Waiting to avoid deadlock.
                if self.transaction_table[locked_txn_id].state == 'Blocked':
                    self.transaction_table[operation.txn_id].state = 'Abort'
                    self.unlock(operation)
                    return 2
                else:
                    self.blocked_operations.append(operation)
                    self.transaction_table[operation.txn_id].state = 'Blocked'
                    return 3

    def write_lock(self, operation):
        '''
        write_lock operation
        :param operation: the operation want to take a read lock
        :return: flag of lock result
        '''
        if operation.item not in self.lock_table.keys():
            self.lock_table[item] = LTRecord('write', operation.txn_id)
            return 1

        elif self.lock_table[operation.item].lock == 'unlock':
            self.lock_table[operation.item].add_write_lock(operation.txn_id)
            return 1

        else:
            record = self.lock_table[operation.item]
            # Judge whether use downgrade lock operation
            if record.lock == 'read' and len(record.locking_txn) == 1 and record.locking_txn[0] == operation.txn_id:
                self.lock_table[operation.item].upgrade_lock()
                return 1
            else:
                # Cautious Waiting to avoid deadlock
                for tid in record.locking_txn:
                    if self.transaction_table[tid].state == 'Blocked':
                        self.transaction_table[operation.txn_id].state = 'Abort'
                        self.unlock(operation)
                        return 2

                self.blocked_operations.append(operation)
                self.transaction_table[operation.txn_id].state = 'Blocked'
                return 3

    def unlock(self, operation):
        '''
        unlock operation
        :param operation: which operation want to unlock all locks
        :return: None
        '''
        i = 0
        # unlock all locks
        for item in self.transaction_table[operation.txn_id].lock_item:
            if self.lock_table[item].lock == 'read':
                self.lock_table[item].unlock_read(operation.txn_id)
            elif self.lock_table[item].lock == 'write':
                self.lock_table[item].unlock_write()
        # activate blocked operations
        for idx, blocked_operation in enumerate(self.blocked_operations):
            if self.transaction_table[blocked_operation.txn_id].state == 'Active' or \
                    blocked_operation.item and self.lock_table.get(blocked_operation.item) and \
                    self.lock_table[blocked_operation.item].lock == 'unlock':
                self.transaction_table[blocked_operation.txn_id].state = 'Active'
                self.operations.insert(i, self.blocked_operations.pop(idx))
                i += 1


def main():
    print('-----------------------------')
    print('---------system log----------')
    print('-----------------------------')
    print()
    parser = argparse.ArgumentParser()
    parser.add_argument('--input', type=str, default=None)
    operation_list = parser.parse_args().input
    scheduler = Scheduler(operation_list)
    scheduler.run_operations()
    print('---------Final Transaction Statement---------')
    for key, value in scheduler.transaction_table.items():
        print('transaction_id:', key, value)


if __name__ == '__main__':
    main()
