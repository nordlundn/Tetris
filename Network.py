import tensorflow as tf
import numpy as np

class Network:
    def __init__(self,
                 sess,
                 state_size,
                 learning_rate,
                 output_size
                ):
        self.sess = sess
        self.s_dim = [None, state_size]
        self.learning_rate = learning_rate
        self.output_size = output_size

        self.writer = None

        self.inputs, self.out_pi, self.out_v = self.create_network()

        # Training

        self.labels_pi = tf.placeholder(tf.float32, [None, self.output_size], name="labels_pi")
        self.labels_v = tf.placeholder(tf.float32, [None, 1], name="labels_v")

        self.loss_pi = tf.nn.sigmoid_cross_entropy_with_logits(labels=self.labels_pi, logits=self.out_pi)
        self.loss_v = tf.squared_difference(self.out_v, self.labels_v)
        self.loss = tf.reduce_mean(self.loss_pi + self.loss_v)

        self.optimize = tf.train.AdamOptimizer(self.learning_rate).minimize(self.loss)

        # Tensorboard

        self.policy_scalar = tf.summary.scalar("Policy loss", tf.reduce_mean(self.loss_pi))
        self.value_scalar = tf.summary.scalar("Value loss", tf.reduce_mean(self.loss_v))
        self.reward_holder = tf.placeholder(tf.float32, (), name="Reward")
        self.reward_scalar = tf.summary.scalar("Reward", self.reward_holder)
        self.trainNum = 0
        self.rewardNum = 0

    def create_network(self):
        inputs = tf.placeholder(tf.float32, self.s_dim, name="inputs")
        norm_inputs = tf.layers.batch_normalization(inputs)

        l1 = tf.layers.dense(inputs=norm_inputs, units=64, activation=tf.nn.relu)
        norm1 = tf.layers.batch_normalization(l1)

        l2 = tf.layers.dense(inputs=norm1, units=64, activation=tf.nn.relu)
        norm2 = tf.layers.batch_normalization(l2)

        l3 = tf.layers.dense(inputs=norm2, units=32, activation=tf.nn.relu)
        norm3 = tf.layers.batch_normalization(l3)

        out_pi = tf.layers.dense(inputs=norm3, units=self.output_size)
        out_v = tf.layers.dense(inputs=norm3, units=1, activation=tf.nn.sigmoid)

        return inputs, out_pi, out_v

    def predict(self, inputs, num_actions):
        # num_actions is a list of elements with the same number of rows as inputs
        # each element corresponds to the number of valid actions for given state
        policies, values = self.sess.run([self.out_pi, self.out_v], feed_dict={self.inputs:inputs})
        policies = list(policies)
        pis = []
        vals = []
        for i, pi in enumerate(policies):
            # pis.append(softmax(pi[:num_actions[i]]))
            pis.append(sigmoid(pi[:int(num_actions[i])]))
            vals.append(float(values[i,0]))
        return pis, vals

    def train(self, inputs, labels_pi, labels_v):
        _, policy_summary, value_summary = self.sess.run([self.optimize, self.policy_scalar, self.value_scalar], feed_dict = {self.inputs:inputs, self.labels_pi:labels_pi, self.labels_v:labels_v})

        self.writer.add_summary(policy_summary, self.trainNum)
        self.writer.add_summary(value_summary, self.trainNum)
        self.trainNum += 1

    def writeReward(self, reward):
        reward_summary = self.sess.run(self.reward_scalar, feed_dict={self.reward_holder:reward})
        self.writer.add_summary(reward_summary, self.rewardNum)
        self.rewardNum += 1

    def setWriter(self, writer):
        self.writer = writer

def softmax(w):
    e = np.exp(np.array(w))
    dist = e/np.sum(e)
    return dist
def sigmoid(w):
    e = np.exp(np.array(w))
    e = e/(1+e)
    return e/np.sum(e)
