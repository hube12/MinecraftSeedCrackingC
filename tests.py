def forward(seed):
    seed^= 0x5DEECE66D
    seed = (seed * 0x5deece66d + 0xb)& ((1 << 48) - 1)
    seed = (seed * 0x5deece66d + 0xb)& ((1 << 48) - 1)
    print("pillar seed",(seed& 0xFFFF0000)>>16,"structure seed",(seed&0xFFFF00000000)>>16|seed&0xFFFF)
    return seed&0xFFFF00000000+seed&0xFFFF,(seed& 0xFFFF0000)>>16
forward(123)
    
def backward(seed,pillar):
    currentSeed = (seed << 16 & 0xFFFF00000000) | (pillar << 16) | (seed & 0xFFFF)
    currentSeed = ((currentSeed - 0xb) * 0xdfe05bcb1365) & 0xffffffffffff
    currentSeed = ((currentSeed - 0xb) * 0xdfe05bcb1365) & 0xffffffffffff
    currentSeed ^= 0x5DEECE66D
    return currentSeed
print(backward(1018962368,1173))
