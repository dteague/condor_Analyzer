''' Partition a list into sublists whose sums don't exceed a maximum 
    using a First Fit Decreasing algorithm. See
    http://www.ams.org/new-in-math/cover/bins1.html
    for a simple description of the method.
'''

    
class Bin(object):
    ''' Container for items that keeps a running sum '''
    def __init__(self):
        self.items = []
        self.sum = 0
    
    def append(self, item):
        self.items.append(item)
        self.sum += item[1]

    def __str__(self):
        ''' Printable representation '''
        return 'Bin(sum=%d, items=%s)' % (self.sum, str(self.items))

    def at(self, i):
        return self.items[i]

    def get_items(self):
        return self.items
        
def pack(values, maxValue):
    values = sorted(values, key=lambda tup: tup[1])
    bins = []
    # values = sorted(values, reverse=True)
    # bins = []
    
    for item in values:
        # Try to fit item into a bin
        for bin in bins:
            if bin.sum + item[1] <= maxValue:
                #print 'Adding', item, 'to', bin
                bin.append(item)
                break
        else:
            # item didn't fit into any bin, start a new bin
            #print 'Making new bin for', item
            bin = Bin()
            bin.append(item)
            bins.append(bin)
    
    return bins

