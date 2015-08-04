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

import java.io.IOException;
import java.util.Arrays;

import de.michaeldenk.yasdi4j.YasdiChannel;
import de.michaeldenk.yasdi4j.YasdiDevice;
import de.michaeldenk.yasdi4j.YasdiDeviceEvent;
import de.michaeldenk.yasdi4j.YasdiDeviceListener;
import de.michaeldenk.yasdi4j.YasdiDriver;
import de.michaeldenk.yasdi4j.YasdiMaster;

public class Test {
	public static void main(String[] args) {
		String config = "";
		if (args.length > 0) {
			config = args[0];
		}
		YasdiMaster master = YasdiMaster.getInstance();
		master.addDeviceListener(new YasdiDeviceListener() {
			public void deviceRemoved(YasdiDeviceEvent e) {
				System.out.println("device removed: " + e.getDevice());
			}

			public void deviceAdded(YasdiDeviceEvent e) {
				System.out.println("device found: " + e.getDevice());
			}
		});
		try {
			master.initialize(config);
		} catch (IOException e2) {
			e2.printStackTrace();
			return;
		}

		// get driver
		YasdiDriver[] drivers = master.getDrivers();
		System.out.println("drivers:" + Arrays.toString(drivers));

		try {
			System.out.println("Setting driver 1 online");
			master.setDriverOnline(drivers[1]);
		} catch (IOException e1) {
			e1.printStackTrace();
			return;
		}

		// detect devices
		try {
			System.out.println("Detecting devices");
			master.detectDevices(5);
		} catch (IOException e) {
			e.printStackTrace();
		}

		// get devices
		YasdiDevice[] devs = master.getDevices();
		System.out.println("devices: " + Arrays.toString(devs));

		// get channels
		System.out.println("channels:");
		for (YasdiDevice d : devs) {
			System.out.println(d.getName());
			YasdiChannel[] chans = d.getChannels(0x090f, 0);
			for (YasdiChannel c : chans) {
				System.out.println("\t" + c.getName() + "(" + c.getUnit()
						+ ") " + c.getAccessRights() + " " + c.getMask());
			}
		}

		// get data
		System.out.println("Getting current values");
		for (YasdiDevice d : devs) {
			System.out.print(d.getName());
			for (YasdiChannel c : d.getChannels(0x090f, 0)) {
				try {
					c.updateValue(5);
					System.out.print("\t"
							+ c.getName()
							+ ": "
							+ (c.hasText() ? c.getValueText() : (c.getValue()
									+ " " + c.getUnit())));
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
			System.out.println();
		}

		master.setDriverOffline(drivers[1]);
		master.shutdown();
	}
}
