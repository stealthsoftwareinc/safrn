/*
 * This file is from the SAFRN package.
 *
 * The following copyright notice is generally applicable:
 *
 *      Copyright (C)
 *         Stealth Software Technologies Commercial, Inc.
 *
 * The full copyright information depends on the distribution
 * of the package. For more information, see the COPYING file.
 * However, depending on the context in which you are viewing
 * this file, the COPYING file may not be available.
 */
package com.stealthsoftwareinc.commercial.safrn.analytics;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.FilenameFilter;
import java.nio.file.Files;
import java.util.HashMap;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Set;
import java.util.stream.Collectors;

import com.stealthsoftwareinc.commercial.safrn.Constants;

public class LatencyLogParser {
	protected String currentBackState = null;
	protected long currentBackTS = 0;
	private Map<String, Long> backLatencies = new HashMap<>();

	private Map<String, Long> frontLatencies = new HashMap<>();
	private String currentFrontState = null;
	private long currentFrontTS = 0;

	protected String outExtension;
	protected String inExtension;
	protected File folder;// input folder to read the log files from

	
	public LatencyLogParser() {
		outExtension = ".latencies";
		inExtension = ".log";
		folder = new File("logs");
		// todo: configuration file instead of hardcoding
	}

	public void calculate(String line) { 
		String[] words = line.trim().split(Constants.DELIMETER);
		String[] tokens = words[words.length - 1].split("=");
		String state = words[4].trim();
		String handler = tokens[0].trim();
		String ts = tokens[1].trim();
		System.out.println(words[4] + "," + tokens[0] + "," + tokens[1]);

		if (handler.contains("BackServerRawHandler")) {
			if (currentBackState == null) {
				this.currentBackState = state;
				this.currentBackTS = Long.parseLong(ts);
			} else if (!state.equalsIgnoreCase(currentBackState)) {
				long prevLatency = this.backLatencies.getOrDefault(currentBackState, 0L);
				long diff = Long.parseLong(ts) - currentBackTS;
				assert diff >= 0;
				this.backLatencies.put(currentBackState, diff + prevLatency);
				this.currentBackState = state;
				this.currentBackTS = Long.parseLong(ts);
			}
		} else if (handler.contains("FrontServerRawHandler")) {
			if (currentFrontState == null) {
				this.currentFrontState = state;
				this.currentFrontTS = Long.parseLong(ts);
			} else if (!state.equalsIgnoreCase(currentFrontState)) {
				long prevLatency = this.frontLatencies.getOrDefault(currentFrontState, 0L);
				long diff = Long.parseLong(ts) - currentFrontTS;
				assert diff >= 0;
				this.frontLatencies.put(currentFrontState, diff + prevLatency);
				this.currentFrontState = state;
				this.currentFrontTS = Long.parseLong(ts);
			}
		} else
			System.out.println("ignoring.." + handler);
	}

	public void logLatencies(BufferedWriter writer, Map<String, Long> latencies) throws Exception {
		Map<String, Long> sortedLatencies = latencies.entrySet().stream()
				.sorted(Entry.<String, Long>comparingByValue().reversed())
				.collect(Collectors.toMap(Entry::getKey, Entry::getValue, (e1, e2) -> e1, LinkedHashMap::new));
		Set<String> keys = sortedLatencies.keySet();
		for (String key : keys) {
			writer.write(key + "," + sortedLatencies.getOrDefault(key, 0L));
			writer.newLine();
		}
		writer.flush();
	}

	public void reInitialize() {
		currentBackState = null;
		currentBackTS = 0;
		backLatencies = new HashMap<>();

		frontLatencies = new HashMap<>();
		currentFrontState = null;
		currentFrontTS = 0;
	}

	public void serialize(String fn) throws Exception {
		BufferedWriter writer = new BufferedWriter(
				new FileWriter(folder.getAbsolutePath() + File.separator + fn + outExtension));
		logLatencies(writer, this.backLatencies);
		logLatencies(writer, this.frontLatencies);
		writer.close();
	}

	public void process() throws Exception {// parse, calculate, serialize and reinitialize  
		FilenameFilter fileNameFilter = new FilenameFilter() {
			public boolean accept(File dir, String name) {
				return name.endsWith(inExtension);
			}
		};

		System.out.println("reading from the folder.." + folder.getAbsolutePath());
		File[] files = folder.listFiles(fileNameFilter);
		for (int i = 0; i < files.length; i++) {
			File file = files[i];
			System.out.println(file.getName() + " Reading from the file.." + file.getAbsolutePath());
			List<String> content = Files.readAllLines(file.toPath());
			for (String line : content) {
				line = line.trim();
				String[] words = line.split(Constants.DELIMETER);
				if (words.length < 5 || !Constants.LATENCY.equalsIgnoreCase(words[1]))
					continue;
				if (words[words.length - 1].contains("=")) {
					calculate(line); 
				}
			}
			String[] fns = file.getName().split("\\.");
			serialize(fns[0]);
			reInitialize();
		}
	}

	public static void main(String[] args) throws Exception {
		// TODO Auto-generated method stub

		LatencyLogParser latencyGenerator = new LatencyLogParser();
		latencyGenerator.process();

	}

}
