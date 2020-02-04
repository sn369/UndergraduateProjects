from torch import nn

class Generator(nn.Module):
    """
    structure:


    """
    def __init__(self):
        super(Generator, self).__init__()
        # 输入生成器的feature map数目为 64
        Gnfm = 64
        self.main = nn.Sequential(
            nn.ConvTranspose2d(100, Gnfm*8, 4, 1, 0, bias=False),
            nn.BatchNorm2d(Gnfm*8),
            nn.ReLU(True),

            nn.ConvTranspose2d(Gnfm*8, Gnfm*4, 4, 2, 1, bias=False),
            nn.BatchNorm2d(Gnfm*4),
            nn.ReLU(True),

            nn.ConvTranspose2d(Gnfm*4, Gnfm*2, 4, 2, 1, bias=False),
            nn.BatchNorm2d(Gnfm*2),
            nn.ReLU(True),

            nn.ConvTranspose2d(Gnfm*2, Gnfm, 4, 2, 1, bias=False),
            nn.BatchNorm2d(Gnfm),
            nn.ReLU(True),

            nn.ConvTranspose2d(Gnfm, 3, 5, 3, 1, bias=False),
            nn.Tanh()
        )

    def forward(self, input):
        return self.main(input)


class Discriminator(nn.Module):
    """
    structure:

    """

    def __init__(self):
        super(Discriminator, self).__init__()
        Dnfm = 64
        self.main = nn.Sequential(
            nn.Conv2d(3, Dnfm, 5, 3, 1, bias=False),
            nn.LeakyReLU(0.2, inplace=True),

            nn.Conv2d(Dnfm, Dnfm*2, 4, 2, 1, bias=False),
            nn.BatchNorm2d(Dnfm*2),
            nn.LeakyReLU(0.2, inplace=True),

            nn.Conv2d(Dnfm*2, Dnfm*4, 4, 2, 1, bias=False),
            nn.BatchNorm2d(Dnfm*4),
            nn.LeakyReLU(0.2, inplace=True),

            nn.Conv2d(Dnfm*4, Dnfm*8, 4, 2, 1, bias=False),
            nn.BatchNorm2d(Dnfm*8),
            nn.LeakyReLU(0.2, inplace=True),

            nn.Conv2d(Dnfm*8, 1, 4, bias=False),
            nn.Sigmoid()
        )

    def forward(self, input):
        return self.main(input).view(-1)