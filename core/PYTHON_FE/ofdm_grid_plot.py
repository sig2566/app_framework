import numpy as np
import matplotlib.pyplot as plt
import matplotlib as mpl

Complex16T = np.dtype([('r', np.int16), ('i', np.int16)])

class ofdm_plot():
    max_sub_curr_num = 275 * 12
    max_slots_num = 2
    max_layers = 4
    max_symbols =  14
    dmrs_code = 0x44
    pdsch_code = 0x55
    rb_num_curr = 12
    
    def split_complex32(self, complex32):
        arr_i = np.asarray(complex32 / (1<<16), dtype='int16')
        arr_r = np.asarray(complex32 & 0xffff, dtype='int16')
        return arr_r,arr_i
        
    def plot_non_zero_constellation(self, complex32_list, fig_name):
        tmp_arr = np.asarray(complex32_list)
        non_zero_arr = tmp_arr[tmp_arr!=0]
        arr_r,arr_i = self.split_complex32(non_zero_arr)
        self.plot_constellation(arr_r, arr_i, fig_name)
        
    def plot_constellation(self, dat_array_r, dat_array_i, fig_name):
        size = np.size(dat_array_r)
        fig, ax = plt.subplots()
#        dat_array.astype(int32)
        plt.plot(dat_array_i, dat_array_r,'ro')
 #       for i in range(0,size):
#            r = (dat_array[i] >> 16) & 0xffff
#            i = dat_array[i] & 0xffff
#            r= dat_array_r[i]
#            i= dat_array_i[i]
#            plt.plot(i,r,'ro')
            
        ax.set_title(fig_name)
        #plt.show()
        plt.pause(0.0001)
        return

    def plot_rx_ant_constellation(self, rx_ant, dat_array_r, dat_array_i, fig_name):
        size = np.size(dat_array_r)
        #fig, ax = plt.subplot(4,4,rx_ant)
        plt.subplot(4,4,rx_ant)
#        dat_array.astype(int32)
        plt.plot(dat_array_i, dat_array_r,'ro')
 #       for i in range(0,size):
#            r = (dat_array[i] >> 16) & 0xffff
#            i = dat_array[i] & 0xffff
#            r= dat_array_r[i]
#            i= dat_array_i[i]
#            plt.plot(i,r,'ro')
            
        #ax.set_title(fig_name)
        plt.title(fig_name)

        # Adjust the subplot layout, because the logit one may take more space
        # than usual, due to y-tick labels like "1 - 10^{-3}"
        plt.subplots_adjust(top=0.92, bottom=0.08, left=0.10, right=0.95, hspace=0.5,wspace=0.5)        
        #plt.show()
        plt.pause(0.0001)
        return
    
    def plot_grid(self, debug_dat_array, fig_name):
        curr_grid_full= np.reshape(debug_dat_array, (ofdm_plot.max_symbols, -1))
        sum_arr = np.sum(curr_grid_full, axis=0)
        rb_indexes = np.where(sum_arr > 0)
        min_currier= np.min(rb_indexes)
        max_currier= np.max(rb_indexes)
        curr_grid = curr_grid_full[:,min_currier:max_currier]
        IM= curr_grid.transpose()
        fig, ax = plt.subplots()
        numrows, numcols = IM.shape

        y = np.arange(min_currier, max_currier, 12)
        x = np.arange(0, numcols, 1)
        X, Y = np.meshgrid(x, y)
        #plt.yticks([y[:]])
        #plt.xticks([x[:]])
        plt.xticks(x)
        plt.yticks(y)
        extent = np.min(x), np.max(x), np.min(y), np.max(y)

        ax.imshow(IM, interpolation='nearest', aspect = 'auto', extent=extent, origin='lower')

        ax.set_title(fig_name)
        plt.grid(True)
        plt.pause(0.0001)
        #plt.show()
        return
    
    def show(self, ofdm_data, grid_debug_data):        
        ofdm_arr= np.asarray(ofdm_data, dtype='uint32')
        grid_arr = np.asarray(grid_debug_data, dtype='int32')
        
        ofdm_arr_r = np.asarray(ofdm_arr / (1<<16), dtype='int16')
        ofdm_arr_i = np.asarray(ofdm_arr * 0xffff, dtype='int16')
        #ofdm_arr_i = ofdm_arr - (ofdm_arr_r * (1<<16))
        
        dmrs_ind = np.where(grid_arr == ofdm_plot.dmrs_code)
        pdsch_ind = np.where(grid_arr == ofdm_plot.pdsch_code)
        dmrs_data_r = ofdm_arr_r[dmrs_ind]
        dmrs_data_i = ofdm_arr_i[dmrs_ind]
        
        pdsch_data_r = ofdm_arr_r[pdsch_ind]
        pdsch_data_i = ofdm_arr_i[pdsch_ind]
        
        str1 = "DMRS Constellation"
        self.plot_constellation(dmrs_data_r, dmrs_data_i ,str1)
        str1 = "PDSCH Constellation"
        self.plot_constellation(pdsch_data_r, pdsch_data_i, str1)
        
        
        ofdm_layer_slot = np.reshape(ofdm_arr, (ofdm_plot.max_slots_num, ofdm_plot.max_layers, -1))
        grid_layer_slot = np.reshape(grid_arr, (ofdm_plot.max_slots_num, ofdm_plot.max_layers, -1))
        for layer in range(0, ofdm_plot.max_layers):
            for cur_slot in range(0, ofdm_plot.max_slots_num):                
                grid_slot = grid_layer_slot[cur_slot][layer]
                # Check if non zero RE are existed
                indx = np.where(grid_slot !=0)
                if np.size(indx) > 0:
                    str1= "Resource Allocation Layer: " + str(layer) + ' Slot: ' + str(cur_slot)
                    self.plot_grid(grid_slot, str1)
        plt.show()   
        return
    
         
        