import tensorflow as tf
import numpy as np
import time
import Tetris
import MCTS

from Tree import *
from Network import *
from replay_buffer import *

with tf.Session() as sess:

    network = Network(sess, 17, .1, 34)
    buffer = ReplayBuffer(1000)

    sess.run(tf.global_variables_initializer())
    filename = "./summary_log/run"+time.strftime("%Y%m%d-%H%M$S")
    writer = tf.summary.FileWriter(filename,sess.graph)
    network.setWriter(writer)

    game = Game(network, 10, 100)
    game.playOut(buffer)

    state_batch, policy_batch, value_batch = buffer.sample_batch(10)
    network.train(state_batch, policy_batch, value_batch)
    game.reset()
