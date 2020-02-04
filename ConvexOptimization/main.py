#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Dec 19 13:53:29 2018

@author: sangna
"""

from __future__ import print_function
import argparse
import torch
import torch.nn as nn
import torch.nn.functional as F
import torch.optim as optim
from torchvision import datasets, transforms
import matplotlib.pyplot as plt


class Logistic(nn.Module):
    def __init__(self):
        super(Logistic, self).__init__()
        self.W = nn.Linear(28*28, 10)

    def forward(self, x):
        x = x.view(-1, 28*28)
        x = self.W(x)
        return F.log_softmax(x, dim=1)


def train(model, device, train_loader, optimizer, epoch):
    model.train()
    for batch_idx, (pics, labels) in enumerate(train_loader):
        pics, labels = pics.to(device), labels.to(device)
        optimizer.zero_grad()
        output = model(pics)
        loss = F.nll_loss(output, labels)
        loss.backward()
        optimizer.step()
        if batch_idx % 10 == 0:
            print('Train Epoch: {} [{}/{} ({:.0f}%)]\tLoss: {:.6f}'.format(
                epoch, batch_idx * len(pics), len(train_loader.dataset),
                100. * batch_idx / len(train_loader), loss.item()))


def test(model, device, test_loader):
    model.eval()
    test_loss = 0
    correct = 0
    with torch.no_grad():
        for pics, labels in test_loader:
            pics, labels = pics.to(device), labels.to(device)
            output = model(pics)
            # 求一个batch总体的损失
            test_loss += F.nll_loss(output, labels, reduction='sum').item()
            # 预测分类
            pred = output.max(1, keepdim=True)[1]
            # 统计分类精度
            correct += pred.eq(labels.view_as(pred)).sum().item()

    test_loss /= len(test_loader.dataset)

    print('\nTest set: Average loss: {:.4f}, Accuracy: {}/{} ({:.0f}%)\n'.format(
        test_loss, correct, len(test_loader.dataset),
        100. * correct / len(test_loader.dataset)))
    return 1. * correct / len(test_loader.dataset), test_loss


def main():
    """
    """
    lr = 0.13
    epochs = 200
    device = torch.device("cuda")
    seed = 1

    parser = argparse.ArgumentParser(
        description='Homework 1 Logistic Regression')
    parser.add_argument('--train-batch-size', type=int, default=60000)
    parser.add_argument('--test-batch-size', type=int, default=10000)
    """
    在SGD优化算法中，选择batch size为训练集和测试集的大小，即为梯度下降法
    因此，命令行中未选择batch size，默认使用梯度下降法
    """
    args = parser.parse_args()

    torch.manual_seed(seed)
    transform = transforms.Compose([
        transforms.ToTensor(),
        transforms.Normalize((0.1307,), (0.3081,))
    ])
    train_loader = torch.utils.data.DataLoader(
        datasets.MNIST('../data', train=True,
                       download=True, transform=transform),
        batch_size=args.train_batch_size,
        shuffle=True
    )
    test_loader = torch.utils.data.DataLoader(
        datasets.MNIST('../data', train=False, transform=transform),
        batch_size=args.test_batch_size,
        shuffle=True
    )

    model = Logistic().to(device)
    optimizer = optim.SGD(model.parameters(), lr=lr, momentum=0)

    accuracy = []
    loss = []
    for epoch in range(1, epochs + 1):
        train(model, device, train_loader, optimizer, epoch)
        a, l = test(model, device, test_loader)
        accuracy.append(a)
        loss.append(l)
    plt.figure()
    plt.title("Accuracy of Predictions")
    plt.plot(accuracy)
    plt.grid()
    plt.savefig("Accuracy")
    plt.figure()
    plt.title("Loss value")
    plt.plot(loss)
    plt.grid()
    plt.savefig("Loss")


if __name__ == '__main__':
    main()
