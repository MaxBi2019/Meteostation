import pandas as pd
import matplotlib
import matplotlib.pyplot as plt
import matplotlib.backends.backend_pdf
from matplotlib.backends.backend_pdf import PdfPages
from matplotlib import dates
import PySimpleGUI as sg 

class reader():
    def __init__(self):
        return None

    def read(self, begin_date, end_date, df):
        data_time = pd.to_datetime(df['Time'])
        data_temp = df['Temperature']
        data_humidity = df['Humidity']
        data_pressure = df['Pressure']
        data_co2 = df['CO2']
        begin_index = list(data_time).index(begin_date)
        end_index = list(data_time).index(end_date)
        return data_time[begin_index:end_index+1], data_temp[begin_index:end_index+1], data_humidity[begin_index:end_index+1], data_pressure[begin_index:end_index+1], data_co2[begin_index:end_index+1]

    def draw(self, data_time, data_temp, data_humidity, data_pressure, data_co2, path_out):
        max_temp = data_temp.max()
        min_temp = data_temp.min()
        mean_temp = data_temp.sum()/len(data_temp)
        if len(data_temp) % 2 == 0:
            median_temp = (list(data_temp)[(len(data_temp)//2)-1] + list(data_temp)[(len(data_temp)//2)])/2
        else:
            median_temp = list(data_temp)[len(data_temp)//2]
        
        max_humidity = data_humidity.max()
        min_humidity = data_humidity.min()
        mean_humidity = data_humidity.sum()/len(data_humidity)
        if len(data_humidity) % 2 == 0:
            median_humidity = (list(data_humidity)[(len(data_humidity)//2)-1] + list(data_temp)[(len(data_humidity)//2)])/2
        else:
            median_humidity = list(data_humidity)[len(data_humidity)//2]

        max_pressure = data_pressure.max()
        min_pressure = data_pressure.min()
        mean_pressure = data_pressure.sum()/len(data_pressure)
        if len(data_pressure) % 2 == 0:
            median_pressure = (list(data_pressure)[(len(data_pressure)//2)-1] + list(data_pressure)[(len(data_pressure)//2)])/2
        else:
            median_pressure = list(data_pressure)[len(data_pressure)//2]

        max_co2 = data_co2.max()
        min_co2 = data_co2.min()
        mean_co2 = data_co2.sum()/len(data_co2)
        if len(data_co2) % 2 == 0:
            median_co2 = (list(data_co2)[(len(data_co2)//2)-1] + list(data_co2)[(len(data_co2)//2)])/2
        else:
            median_co2 = list(data_co2)[len(data_co2)//2]

        with PdfPages(path_out) as export_pdf:
            pageDimensions = plt.figure(figsize=(6.40,4.8))
            pageDimensions.clf()
            txt = "Weather report\nfrom " + str(pd.to_datetime(list(data_time)[0])) + " to " + str(pd.to_datetime(list(data_time)[-1]))
            pageDimensions.text(0.5,0.5,txt, transform=pageDimensions.transFigure, size=16, ha="center")
            export_pdf.savefig()
            plt.close()

            pageDimensions = plt.figure(figsize=(6.40,4.8))
            pageDimensions.clf()
            plt.subplot().set_axis_off()
            plt.tight_layout()
            rows = ('Temperature', 'Humidity', 'Pressure', 'CO2')
            columns = ['Maximum', 'Minimum', 'Mean', 'Median']
            cells = [[max_temp, min_temp, mean_temp, median_temp], [max_humidity, min_humidity, mean_humidity, median_humidity], [max_pressure, min_pressure, mean_pressure, median_pressure], [max_co2, min_co2, mean_co2, median_co2]]
            for i in range(len(cells)):
                for j in range(len(cells[i])):
                    cells[i][j] = round(cells[i][j], 2)
            table = plt.table(cellText=cells, rowLabels=rows, colLabels=columns, loc ='center', cellLoc='center', bbox=[0.3, 0.3, 0.5, 0.5], colWidths=[0.1 for x in columns])
            export_pdf.savefig()
            plt.close()

            plt.plot(data_time, data_temp[:], linestyle='solid', marker='.')
            plt.gcf().autofmt_xdate()
            date_format = dates.DateFormatter('%d-%m-%Y')
            plt.gca().xaxis.set_major_formatter(date_format)
            plt.title('Temperature from ' + str(pd.to_datetime(list(data_time)[0])) + " to " + str(pd.to_datetime(list(data_time)[-1])))
            plt.xlabel('Time')
            plt.ylabel('Temperature')
            plt.minorticks_on()
            plt.grid(True)
            
            export_pdf.savefig()
            plt.close()

            plt.plot_date(data_time, data_humidity, linestyle='solid', marker='.')
            plt.gcf().autofmt_xdate()
            plt.gca().xaxis.set_major_formatter(date_format)
            plt.title('Humidity from ' + str(pd.to_datetime(list(data_time)[0])) + " to " + str(pd.to_datetime(list(data_time)[-1])))
            plt.xlabel('Time')
            plt.ylabel('Humidity')
            plt.minorticks_on()
            plt.grid(True)
            export_pdf.savefig()
            plt.close()

            plt.plot_date(data_time, data_pressure, linestyle='solid', marker='.')
            plt.gcf().autofmt_xdate()
            date_format = dates.DateFormatter('%d-%m-%Y')
            plt.gca().xaxis.set_major_formatter(date_format)
            plt.title('Pressure from ' + str(pd.to_datetime(list(data_time)[0])) + " to " + str(pd.to_datetime(list(data_time)[-1])))
            plt.xlabel('Time')
            plt.ylabel('Pressure')
            plt.minorticks_on()
            plt.grid(True)
            export_pdf.savefig()
            plt.close()

            plt.plot_date(data_time, data_co2, linestyle='solid', marker='.')
            plt.gcf().autofmt_xdate()
            plt.gca().xaxis.set_major_formatter(date_format)
            plt.title('CO2 from ' + str(pd.to_datetime(list(data_time)[0])) + " to " + str(pd.to_datetime(list(data_time)[-1])))
            plt.xlabel('Time')
            plt.ylabel('CO2')
            plt.minorticks_on()
            plt.grid(True)
            export_pdf.savefig()
            plt.close()

    def run(self):
        sg.theme('BlueMono')      
        path_data = sg.popup_get_file("Choose Weather Data File",
            title = "Choose Weather Data File",
            file_types = ((".csv", '.csv'),))
        if path_data != "" and path_data != None:
            df = pd.read_csv(path_data)
            data_time = pd.to_datetime(df['Time'])
            date = list()
            for i in range(len(data_time)):
                date.append(str(data_time[i]))
            layout = [ 
                [sg.Text('Please enter the time frame you want to generate')],
                [sg.Text('Start Date', size =(9, 1)), sg.Spin(date)], 
                [sg.Text('End Date', size =(9, 1)), sg.Spin(date)], 
                [sg.Submit('Submit', tooltip='Click to create pdf file'), sg.Cancel()] 
            ] 
            
            window = sg.Window('PDF report generator', layout) 
            event, values = window.read() 
            if event == 'Submit':
                data_time, data_temp, data_humidity, data_pressure, data_co2 = self.read(pd.to_datetime(values[0]), pd.to_datetime(values[1]), df)
                path_out = sg.popup_get_file("Save File",
                    save_as = True,
                    title = "Save File",
                    file_types = ((".pdf", '.pdf'),))
                window.close()
                if path_out != "" and path_out != None:
                    self.draw(data_time, data_temp, data_humidity, data_pressure, data_co2, path_out)

if __name__ == "__main__":
    new_class = reader()
    new_class.run()
