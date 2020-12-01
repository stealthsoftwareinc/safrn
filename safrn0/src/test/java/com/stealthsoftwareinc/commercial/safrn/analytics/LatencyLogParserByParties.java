package com.stealthsoftwareinc.commercial.safrn.analytics;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.FilenameFilter;
import java.nio.file.Files;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Vector;

import com.stealthsoftwareinc.commercial.safrn.BackServerRawState;
import com.stealthsoftwareinc.commercial.safrn.Constants;

public class LatencyLogParserByParties extends LatencyLogParser {
	private Map<Integer, LocalParty> localParties;
	private boolean aggregateMode;

	public LatencyLogParserByParties() {
		super();
		outExtension = ".csv";
		localParties = new HashMap<>();
		aggregateMode = false;
	}

	 

	@Override
	public void calculate(String line) {
		String[] words = line.trim().split(Constants.DELIMETER);
		if (words.length < 7 || !line.contains("BackServerRawHandler"))
			return;
		String state = words[4].trim();
		String[] tsTokens = words[words.length - 1].split("=");
		String[] remoteTokens = words[words.length - 2].split("=");
		String[] localTokens = words[words.length - 3].split("=");
		String handler = tsTokens[0].trim();
		Long ts = Long.parseLong(tsTokens[1].trim());
		Integer remoteId = Integer.parseInt(remoteTokens[1].trim());
		Integer localId = Integer.parseInt(localTokens[1].trim());

		System.out.println(words[4] + "," + tsTokens[0] + "," + tsTokens[1] + "," + localId + "," + remoteId);
		// 13:29:21.103813;LATENCY;BackServerRawHandler;inside for
		// loop;G_A_M_YI_0;Id()=12;localparty=3;remoteparty=0;tsBackServerRawHandler=1579555761103
		if (handler.contains("BackServerRawHandler") && remoteId != -1) {
			LocalParty lparty = localParties.getOrDefault(localId, new LocalParty());
			lparty.calculate(state, remoteId, localId, ts);
			localParties.put(localId, lparty);

		} else
			System.out.println("ignoring.." + handler);
	}

	public void reInitialize() {
		if (!aggregateMode) {
			// todo
		}
		localParties = new HashMap<>();
	}

	public void serialize(String fn) throws Exception {
		if (aggregateMode)
			return; //todo main will reset and call serialize again
		for (Entry<Integer, LocalParty> entry : localParties.entrySet()) {
			BufferedWriter writer = new BufferedWriter(
					new FileWriter(folder.getAbsolutePath() + File.separator + fn + "-LP"+entry.getKey() + outExtension));
			Vector<Long>[] latencies = entry.getValue().getLatencies();
			int rid = 0;
			writer.write("   ");
			for (BackServerRawState state : BackServerRawState.values()) {
				writer.write(String.format(",%8s", state.name()));

			}
			writer.newLine();
			//writer.newLine();
			writer.flush();
			for (Vector<Long> rowLatencies : latencies) {
				writer.write(rid + "  ,");
				for (int index = 0; index < rowLatencies.size(); index++) {
					writer.write("" + String.format("%8s", rowLatencies.elementAt(index)));
					writer.write(",");
				}
				rid++;
				writer.newLine();
			}
			writer.flush();
			writer.close();
		}
	}

	public void process() throws Exception {
		super.process();
	}

	public static void main(String[] args) throws Exception { // todo: cmd line args
		System.out.println("==============");
		LatencyLogParserByParties latency = new LatencyLogParserByParties();
		latency.process();
		System.out.println("==============");
	}

	private class LocalParty {
		int nrows = 7;// nRemoteParties (atmost 7)
		int maxSeen = -1;// we can have upto 7 remoteparties; holds max count/id
		Vector<Long>[] latencies;// (end-beg) from following vector;
		Vector<Long>[] milliSeconds;// value from the log; intermediate cache
		Vector<Integer> recentIndex; // index into each row/remoteparty of milliseconds vector. When a new state
										// comes in, deduct that value from recentindex's slot value and add that value
										// to the right slot

		
		@SuppressWarnings("unchecked")
		public LocalParty() {
			recentIndex = new Vector<>();
			latencies = (Vector<Long>[]) new Vector[7];
			milliSeconds = (Vector<Long>[]) new Vector[7];
			for (int i = 0; i < nrows; i++) {
				latencies[i] = new Vector<>();
				milliSeconds[i] = new Vector<>();
				recentIndex.add(-1);
				int nc = BackServerRawState.values().length;
				for (int j = 0; j < nc; j++) {
					latencies[i].add(0L);
					milliSeconds[i].add(0L);
				}
			}
		}

		public Vector<Long>[] getLatencies() {
			return latencies;
		}

		public void calculate(String state, int remoteId, int localId, long ts) {
			// 13:29:21.103813;LATENCY;BackServerRawHandler;inside for
			// loop;G_A_M_YI_0;Id()=12;localparty=3;remoteparty=0;tsBackServerRawHandler=1579555761103
			BackServerRawState nextState = BackServerRawState.valueOf(state.trim());
			if (recentIndex.get(remoteId) == -1) {
				milliSeconds[remoteId].set(nextState.ordinal(), ts);
				recentIndex.set(remoteId, nextState.ordinal());
			} else {//// if it is different stateid for same remoteid: diff TS and add to latencies
				// ignore if it is same as previous stateid
				int mostRecentState = recentIndex.elementAt(remoteId);
				if (mostRecentState == nextState.ordinal())
					return;
				long diff = ts - milliSeconds[remoteId].get(mostRecentState);
				long prevLatency = latencies[remoteId].elementAt(mostRecentState);
				System.out.println("latency=" + diff + " and prevlatency " + prevLatency + " for "
						+ BackServerRawState.values()[mostRecentState]);
				latencies[remoteId].set(mostRecentState, diff + prevLatency);
				recentIndex.set(remoteId, nextState.ordinal());
				milliSeconds[remoteId].set(nextState.ordinal(), ts);
				milliSeconds[remoteId].set(mostRecentState, 0L);// reset back to zero (helps testing)		
			}
		}

	}

}