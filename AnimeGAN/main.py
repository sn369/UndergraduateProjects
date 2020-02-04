import torch
import torchvision
import torch.nn as nn
import torch.nn.functional as F
from torchvision import datasets
from torchvision import transforms
from torchvision.utils import save_image
from torch.autograd import Variable
from torchnet.meter import AverageValueMeter
import os
import ipdb
import tqdm
from model import Generator, Discriminator
from visualize import Visualizer


os.environ.setdefault("CUDA_VISIBLE_DEVICES","1")

batch_size = 256
num_epoch = 200
z_dimension = 100
img_resolution = 96
data_path = 'newdata/'  # 注意数据集图片存放的位置是/newdata/faces/1.jpg,2.jpg,3.jpg...

img_transform = transforms.Compose([
        transforms.Resize(img_resolution),
        transforms.CenterCrop(img_resolution),
        transforms.ToTensor(),
        transforms.Normalize((0.5, 0.5, 0.5), (0.5, 0.5, 0.5))
])

dataset = datasets.ImageFolder(data_path, transform = img_transform)
dataloader = torch.utils.data.DataLoader(dataset,
                            batch_size=batch_size,
                            shuffle=True,
                            num_workers=4,
                            drop_last=True
                            )

G, D = Generator(), Discriminator()
map_location = lambda storage, loc: storage
D.load_state_dict(torch.load('checkpoints/D_199.pth', map_location=map_location))
G.load_state_dict(torch.load('checkpoints/G_199.pth', map_location=map_location))
if torch.cuda.is_available():
    D = D.cuda()
    G = G.cuda()

G_optimizer = torch.optim.Adam(G.parameters(), lr = 2e-4, betas=(0.5, 0.999))
D_optimizer = torch.optim.Adam(D.parameters(), lr = 2e-4, betas=(0.5, 0.999))
criterion = nn.BCELoss().cuda()

true_labels = torch.ones(batch_size).cuda()
fake_labels = torch.zeros(batch_size).cuda()
fix_noises = torch.randn(batch_size, z_dimension, 1, 1).cuda()
noises = torch.randn(batch_size, z_dimension, 1, 1).cuda()

errorD_meter = AverageValueMeter()
errorG_meter = AverageValueMeter()


def train(**kwargs):
    vis = Visualizer('GAN')     # 可视化
    for epoch in range(num_epoch):
        for i, (img, _) in tqdm.tqdm(enumerate(dataloader)):
            real_img = img.cuda()

            # train D
            D_optimizer.zero_grad()

            # 将 real_img 判断成 1
            output = D(real_img)
            D_loss_real = criterion(output, true_labels)
            D_loss_real.backward()

            # 将 fake_img 判断成 0
            noises.data.copy_(torch.randn(batch_size, z_dimension, 1, 1))
            fake_img = G(noises).detach()  # draw fake pic
            output = D(fake_img)
            D_loss_fake = criterion(output, fake_labels)
            D_loss_fake.backward()
            D_optimizer.step()

            D_loss = D_loss_real + D_loss_fake
            errorD_meter.add(D_loss.item())

            if i % 5 == 0:  # train G every 5 batches
                G_optimizer.zero_grad()
                noises.data.copy_(torch.randn(batch_size, z_dimension, 1, 1))
                fake_img = G(noises)
                output = D(fake_img)
                G_loss = criterion(output, true_labels)
                G_loss.backward()
                G_optimizer.step()
                errorG_meter.add(G_loss.item())

        if (epoch + 1) % 10 == 0:   # save model every 10 epochs

            if os.path.exists('/tmp/debugGAN'):
                ipdb.set_trace()
            fix_fake_imgs = G(fix_noises)
            vis.images(fix_fake_imgs.detach().cpu().numpy()[:64] * 0.5 + 0.5, win='fake image')
            vis.images(real_img.data.cpu().numpy()[:64] * 0.5 + 0.5, win='real image')
            vis.plot('errorD', errorD_meter.value()[0])
            vis.plot('errorG', errorG_meter.value()[0])

            torchvision.utils.save_image(fix_fake_imgs.data[:64], '%s/%s.png' % ('imgs', epoch), normalize=True,
                                range=(-1, 1))
            torch.save(D.state_dict(), 'checkpoints/D_%s.pth' % epoch)
            torch.save(G.state_dict(), 'checkpoints/G_%s.pth' % epoch)
            errorD_meter.reset()
            errorG_meter.reset()


def generate(**kwargs):

    G, D = Generator().eval(),Discriminator().eval()
    noises = torch.randn(512, z_dimension, 1, 1).normal_(0, 1)  # search best from 521 imgs
    noises = noises.cuda()

    map_location = lambda storage, loc: storage
    D.load_state_dict(torch.load('checkpoints/D_199.pth', map_location=map_location))   # use trained model
    G.load_state_dict(torch.load('checkpoints/G_199.pth', map_location=map_location))
    D.cuda()
    G.cuda()

    fake_img = G(noises)
    scores = D(fake_img).detach()

    indexs = scores.topk(64)[1]
    result = []
    for i in indexs:
        result.append(fake_img.data[i])
    # save the 64 best imgs
    torchvision.utils.save_image(torch.stack(result), 'output/final4.png', normalize=True, range=(-1, 1))

if __name__=='__main__':
    import fire
    fire.Fire()