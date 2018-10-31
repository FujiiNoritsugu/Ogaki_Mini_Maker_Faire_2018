import tensorflow as tf
from sklearn import datasets
import numpy as np
from sklearn.model_selection import train_test_split
from sklearn.utils import shuffle
import os
import socket
from time import sleep
import traceback

class DNN(object):
    def __init__(self, n_in, n_hiddens, n_out):
        # initialize
        self.n_in = n_in
        self.n_hiddens = n_hiddens
        self.n_out = n_out
        self.weights = []
        self.biasses = []
        self._x = None
        self._t = None
        self._keep_prob = None
        self._sess = None
        self._history = {
            'accuracy':[],
            'loss':[]
        }
        self._accuracy = None
        self._infer = None
    
    def weight_variable(self, shape, index):
        initial = tf.truncated_normal(shape, stddev=0.01)
        name='w'+str(index)
        return tf.Variable(initial, name)
    
    def bias_variable(self, shape, index):
        initial = tf.zeros(shape)
        name='b'+str(index)
        return tf.Variable(initial, name)
    
    def inference(self, x, keep_prob):
        # define model
        for i, n_hidden in enumerate(self.n_hiddens):
            if i == 0:
                input = x
                input_dim = self.n_in
            else:
                input = output
                input_dim = self.n_hiddens[i-1]
            
            self.weights.append(self.weight_variable([input_dim, n_hidden], i))
            self.biasses.append(self.bias_variable([n_hidden], i))
            
            h = tf.nn.relu(tf.matmul(
                input, self.weights[-1]) + self.biasses[-1])
            output = tf.nn.dropout(h, keep_prob)
      
        self.weights.append(self.weight_variable([self.n_hiddens[-1], self.n_out], len(self.n_hiddens)))
        self.biasses.append(self.bias_variable([self.n_out], len(self.n_hiddens)))
        
        y = tf.nn.softmax(tf.matmul(
            output, self.weights[-1]) + self.biasses[-1])       
        return y
    
    def loss(self, y, t):
        cross_entropy = tf.reduce_mean(-tf.reduce_sum(t * tf.log(tf.clip_by_value(y, 1e-10, 1.0)),
                                                     reduction_indices=[1]))
        return cross_entropy
    
    def training(self, loss):
        optimizer = tf.train.GradientDescentOptimizer(0.01)
        train_step = optimizer.minimize(loss)
        return train_step
    
    def accuracy(self, y, t):
        correct_prediction = tf.equal(tf.argmax(y, 1), tf.argmax(t, 1))
        accuracy = tf.reduce_mean(tf.cast(correct_prediction, tf.float32))
        return accuracy
        
    def fit(self):
        x = tf.placeholder(tf.float32, shape=[None, self.n_in])
        t = tf.placeholder(tf.float32, shape=[None, self.n_out])
        keep_prob = tf.placeholder(tf.float32)
        
        self._x = x
        self._t = t
        self._keep_prob = keep_prob
         
        y = self.inference(x, keep_prob)
        self._infer = y
        loss = self.loss(y, t)
        train_step = self.training(loss)
        self._accuracy = self.accuracy(y, t)
        
        saver = tf.train.Saver()
        sess = tf.Session()
        saver.restore(sess, MODEL_DIR + '/model.ckpt')
        
        self._sess = sess
        
        return
        # process for learning
        
    def evaluate(self, X_test, Y_test):
        
        return self._accuracy.eval(session=self._sess, feed_dict={
            self._x: X_test,
            self._t: Y_test,
            self._keep_prob: 1.0
        })
    
    def predict(self, param_x):
        return self._infer.eval(session=self._sess, feed_dict={
            self._x: param_x,
            self._keep_prob: 1.0
        })

if __name__ == '__main__':
    #モデルのロード
    MODEL_DIR = os.path.join(os.path.dirname("__file__"), 'model_100')
    if os.path.exists(MODEL_DIR) is False:
        os.mkdir(MODEL_DIR)
    
    OUTPUT_LAYER = 4

    model = DNN(n_in=1000,
           n_hiddens=[100, 100, 100],
           n_out=OUTPUT_LAYER)
    model.fit()

    #ソケットを介してデータのやり取りを行う
    s = socket.socket()

    port = 5000
    s.bind(('', port))

    while True:
        print('listening')
        s.listen(5)
        c, addr = s.accept()
        while True:
            print('receiving')
            receive_msg = c.recv(4096).decode('utf-8')
            #print(receive_msg)
            try:
                result_list = [receive_msg.strip().split(',')]
                pattern = model.predict(result_list)
                #print("pattern = " , pattern)    
                res = str(np.argmax(pattern, axis=1)[0])
                print("type = ", res)
                c.send(bytes(res + '\n', 'utf-8'))
            except:
                print("Exception occured !!")
                traceback.print_exc()
                break
            sleep(1)
        c.close()
    s.close()
