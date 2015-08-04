/*
 * yasdi4j -- Java Binding for YASDI
 * Copyright (c) 2008 Michael Denk <code@michaeldenk.de>
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

import java.text.DecimalFormat;
import de.michaeldenk.yasdi4j.*;

public class SimpleLogger {
	public static void main(String[] args) throws Exception {
		final String yasdiIni = "yasdi.ini"; // YASDI ini file
		final int nrDevices = 5; // the number of devices in the plant
		final int maxValueAge = 5; // seconds; see YASDI manual

		/* get and initialize the master */
		YasdiMaster master = YasdiMaster.getInstance();
		master.initialize(yasdiIni);

		/* register a listener for device events */
		master.addDeviceListener(new YasdiDeviceListener() {
			public void deviceAdded(YasdiDeviceEvent e) {
				System.out.println("New device found: " + e.getDevice());
			}

			public void deviceRemoved(YasdiDeviceEvent e) {
				// skip
			}
		});

		/* get available drivers and set all drivers online */
		YasdiDriver[] drivers = master.getDrivers();
		for (YasdiDriver d : drivers) {
			master.setDriverOnline(d);
		}

		/* detect and get devices */
		master.detectDevices(nrDevices);
		YasdiDevice[] devices = master.getDevices();

		/* print all spot value channels */
		DecimalFormat df = new DecimalFormat("0.00");
		for (YasdiDevice d : devices) {
			System.out.print(d.getName());
			for (YasdiChannel c : d.getSpotValueChannels()) {
				/* request current channel value */
				c.updateValue(maxValueAge);

				String value = c.hasText() ? c.getValueText()
						: df.format(c.getValue());
				System.out.print(" " + c.getName() + ": " + value + c.getUnit());
			}
			System.out.println();
		}

		/* clean up */
		/* set drivers offline */
		for (YasdiDriver d : drivers) {
			master.setDriverOffline(d);
		}
		/* free the master resources */
		master.shutdown();
	}
}
