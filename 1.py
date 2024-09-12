nosta = ['http://89080']
nosta[0] = nosta[0].removeprefix('http://')
#print(nosta)
s = 0
nosta.append('adc')
nosta.insert(0,'ppt')

for i in range(0,5):
    s+= i

print(nosta)
    
del nosta[0]
a = nosta.pop()
#nosta.remove('cccd')
print(s,f'{nosta} ,{a}')
print(nosta)