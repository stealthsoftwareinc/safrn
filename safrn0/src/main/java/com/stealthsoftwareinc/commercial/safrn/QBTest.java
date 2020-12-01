package com.stealthsoftwareinc.commercial.safrn;

import com.stealthsoftwareinc.commercial.safrn.QueryBuilder;

import com.stealthsoftwareinc.sst.JdbcSubprotocol;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.io.Writer;
import java.util.Scanner;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;
import java.util.List;
import java.util.concurrent.Callable;

import picocli.CommandLine;
import picocli.CommandLine.Command;
import picocli.CommandLine.Option;
import picocli.CommandLine.Parameters;

/**
 * 
 * @author Shaan Mathur
 *
 * @brief   QBTest provides a CLI to convert SAFRN REST queries into their corresponding
 *          SQL queries that safrn.QueryBuilder builds. The picocli source code is included
 *          for the argument parsing logic.
 */
@Command(name = "qbtest", mixinStandardHelpOptions = true, version = "qbtest 1.0",
         description = "QBTest provides a CLI to convert SAFRN REST queries into their corresponding SQL queries built by safrn.QueryBuilder.")
public class QBTest implements Callable<Integer> {

  @Parameters(paramLabel = "QUERIES", arity = "1..*", description = "one or more queries to convert into coresponding SQL query")
  private String[] queries;
  
  @Option(names = {"-d", "--domain-add"}, required = true, paramLabel = "DOMAIN_NAMES", description = "populates domainNames for QueryBuilder")
  private HashMap<String, String> _domainNames;
  
  private HashMap<String, ArrayList<String>> domainNames = new HashMap<String, ArrayList<String>>();

  @Option(names = {"-l", "--local", "--local-party"}, required = true, paramLabel = "LOCAL_PARTY", description = "specifies localParty parameter")
  private int localParty;
  
  @Option(names = {"-r", "--remote", "--remote-party"}, required = true, paramLabel = "REMOTE_PARTY", description = "specifies remoteParty parameter")
  private int remoteParty;
  
  @Option(names = {"-t", "--table", "--table-name"}, paramLabel = "TABLE_NAME", description = "table name to be queried")
  private String tableName = "QBTestTable";
  
  @Option(names = {"-f", "--file", "--input-file"}, paramLabel = "INPUT_FILE_NAME", description = "input file of REST queries")
  private String inputFileName;

  @Option(names = {"-of", "--output-file"}, paramLabel = "OUTPUT_FILE_NAME", description = "output file of SQL queries")
  private String outputFileName;
  
  private Writer output;

  public static void main(String[] args) {
    int exitCode = new CommandLine(new QBTest()).execute(args); // so picocli can assign member variables according to args
    System.exit(exitCode);
  }

  private boolean initOutputWriter() {
    try {
      this.output = this.outputFileName == null ? new OutputStreamWriter(System.out) : new PrintWriter(this.outputFileName); 
    } catch (FileNotFoundException e) {
      System.err.printf("File '%s' cannot be found.", this.outputFileName);
      return false;
    }
    
    return true;
  }
  
  /**
   * @brief Appends each query in File this.inputFileName into this.queries, or leaves it alone if no file is specified.
   * 
   * @return true   if either the files contents are loaded or there was no file specified
   * @return false  if some kind of failure occurs (an error message prints the specific problem)
   */
  private boolean loadFileContents() {
    
    if (this.inputFileName == null) {
      return true; // nothing to do
    }
    
    File inputFile = new File(this.inputFileName);
    if (!inputFile.exists()) {
      System.err.printf("File '%s' does not exist.", this.inputFileName);
      return false;
    }
    
    Scanner scan = null;
    try {
      scan = new Scanner(inputFile);
    } catch (FileNotFoundException e) {
      System.err.printf("File '%s' cannot be found even though it exists.", this.inputFileName);
      return false;
    }
    
    ArrayList<String> queriesList = new ArrayList<String>(Arrays.asList(this.queries));
    while (scan.hasNextLine()) {
      queriesList.add(scan.nextLine());
    }
    scan.close();
    this.queries = queriesList.toArray(this.queries);
      
    return true;
  }
  
  /**
   * @brief   Initializes variables. Cannot be done in constructor because picocli requires a constructed object to begin.
   * @return  true if initialization succeeds, and false otherwise.
   */
  private boolean init() {
    return this.initDomainNames() && this.loadFileContents() && this.initOutputWriter();
  }
  
  /**
   *  @brief   This function is invoked once all instance variables are assigned according to the command line args.
   *           This method invokes a subroutine to parse the query arguments, and then uses the returned argument
   *           dictionary to determine the constructor arguments for QueryBuilder.
   */
  @Override
  public Integer call() throws Exception {
    
    if (!this.init())
      System.exit(1);
    
    // All queries are converted into a mapping from query parameters to their corresponding values
    Map<String, String>[] queries_args = getAllQueryArgs();
    if (queries_args == null)
      System.exit(1);
    
    // Build the SQL Query for each query provided at the command line
    for (Map<String, String> query_args : queries_args) {
      
      // loanlinkbool defaults to false
      boolean loanlinkbool = false;
      if (query_args.containsKey("loanlinkbool")){
        loanlinkbool = query_args.get("loanlinkbool").equals("true");
      }
      
      List<String> dvList = query_args.containsKey("dv") ? Arrays.asList(query_args.get("dv").split("\\+", 0)) : new ArrayList<String>();
      List<String> ivList = query_args.containsKey("iv") ? Arrays.asList(query_args.get("iv").split("\\+", 0)) : new ArrayList<String>();
      
      QueryBuilder qb = new QueryBuilder(  query_args.get("analysis"),
                                          loanlinkbool,
                                          this.domainNames,
                                          dvList,
                                          ivList,
                                          this.localParty,
                                          this.remoteParty,
                                          this.tableName,
                                          JdbcSubprotocol.MYSQL);
      this.output.write(qb.buildQuery() + "\n");
      
    }

    this.output.close(); // this may close System.out if no file specified. This is fine since we are done
    return null;
  }
  
  /**
   * 
   * @return  An array of HashMaps. There is one HashMap for every query provided in the command line,
   *          each HashMap mapping the query's key=value arguments
   */
  private Map<String, String>[] getAllQueryArgs() {
    Map<String, String>[] queries = (Map<String, String>[]) new HashMap[this.queries.length];
    
    for (int i = 0; i < this.queries.length; i++) {
      queries[i] = getQueryArgs(this.queries[i]);      
    }
    
    return queries;
  }
  
  /**
   * 
   * @param query                     The query to be parsed
   * 
   * @return null                     If query is invalid.
   * @return HashMap<String,String>   Mapping from parameters to arguments in query.
   */
  private static Map<String, String> getQueryArgs(String query) {
    
    // query must be of form /query?key1=val1&key2=val2.0+val2.1
    if (!query.matches("\\/query\\?([^?+&/=]+=[^?&/=]+)(&[^?+&/=]+=[^?/&=]+)*")) {
      System.err.printf("Query '%s' is syntactically malformed.\n", query);
      return null;
    }
    
    Map<String, String> arg_map = new HashMap<String, String>();
    
    String query_body = query.substring(query.indexOf('?') + 1).trim();
    String[] assignments = query_body.split("&", 0); // ["key1=val1", "key2=val2.0+val2.1"]
    
    for (String assignment : assignments) {
      String[] key_val = assignment.split("=", 0); // ["key1", "val1"]
      if (arg_map.containsKey(key_val[0])) {
        System.err.printf("Query '%s' contains duplicate key '%s'.\n", query, key_val[0]);
        return null;
      }
      arg_map.put(key_val[0], key_val[1]);
    }
    
    if (!isValidQuery(arg_map)) {
      System.err.println("Query is invalid.");
      return null;
    }
    
    return arg_map;    
  }
  
  
  private static final Set<String> parameters;
  static {
    parameters = new HashSet<String>();
    parameters.add("analysis");
    parameters.add("dv");
    parameters.add("iv");
    parameters.add("countonlymean");
    parameters.add("loanlink");
    parameters.add("timing");
    parameters.add("debug");
  }
    
  private static boolean isValidQuery(Map<String, String> query) {
    for (String key : query.keySet()) {
      if (!parameters.contains(key)) {
        System.err.printf("Query '%s' uses invalid key '%s'.\n", query, key);
        return false;
      }
    }
    
    if (!query.containsKey("analysis")) {
      System.err.printf("Query '%s' must include analysis parameter.\n", query);
      return false;
    }
    
    String analysis = query.get("analysis");
    
    if (!analysis.equals("mean") && !analysis.equals("freq")) {
      System.err.printf("Query '%s' must have analysis=mean or analysis=freq.\n", query);
      return false;
    }
    
    boolean contains_dv = query.containsKey("dv");
    
    if (analysis.equals("mean") && !contains_dv) {
      System.err.printf("Query '%s' has analysis=mean but no 'dv' argument provided.\n", query);
      return false;
    }
    
    if (contains_dv && !query.get("dv").equals("Income")) {
      System.err.printf("Query '%s' does not have dv=Income.\n", query);
      return false;
    }
    
    if (query.containsKey("iv")) {
      String[] args = query.get("iv").split("\\+");
      
      // No duplicate arguments, and only values can any subset of {Attrib_A, Attrib_B, Group_X}
      Set<String> args_set = new HashSet<String>();
      for (String arg : args) {
        if (!arg.equals("Attrib_A") && !arg.equals("Group_X") && !arg.equals("Attrib_B")) {
          System.err.printf("Query '%s' has one of its iv values as '%s', but iv can only be "
              + "Attrib_A, Attrib_B, or Group_X.\n", query, arg);
          return false;
        }
        
        if (args_set.contains(arg)) {
          System.err.printf("Query '%s' uses duplicate argument '%s' for iv.\n", query, arg);
          return false;
        }
        
        args_set.add(arg);
      }
    }
    
    if (query.containsKey("countonlymean")) {
      
      if (!analysis.equals("mean")) {
        System.err.printf("Query '%s' uses countonlymean, but this can only be used if analysis=mean.\n", query);
        return false;
      }
      
      String countonlymean = query.get("countonlymean");
      
      if (!countonlymean.equals("0") && !countonlymean.equals("1") && !countonlymean.equals("2")) {
        System.err.printf("Query '%s' uses countonlymean, but this can only take on values 0, 1, or 2.\n", query);
        return false;
      }
    }
    
    if (query.containsKey("loanlink")) {
      String loanlink = query.get("loanlink");
      if (!loanlink.equals("0") && loanlink.equals("1")) {
        System.err.printf("Query '%s' contains loanlink, but this can only take on values 0 or 1.\n", query);
        return false;
      }
    }
    
    return true;
  }
    
  /**
   * Populates domainNames with elements from _domainNames. This level of indirection is necessary because
   * the picocli library for CLI arguments can't build a Map<String,ArrayList<String>>. Instead, the 
   * Map<String,String> is filled by picocli and then we convert it into Map<String, ArrayList<String>>.
   * 
   * @return true iff all domainNames can be converted (no syntax errors)
   */
  private boolean initDomainNames() {
    for (Map.Entry<String, String> entry : this._domainNames.entrySet()) {
      
      // Convert the values in _domainNames from Strings (e.g. "{alice, bob, eve}" or "[chance, travis, kendrick"]
      // to an ArrayList of Strings (e.g. {"alice", "bob", "eve"} or {"chance", "travis", "kendrick"})
      
      String value = entry.getValue().trim();

      // List must be nonempty, comma-separated, enclosed in a single pair of braces ({}, []) 
      if (!value.matches("(\\{[^\\}\\]\\{\\[,]+(,[^\\}\\]\\{\\[,]+)*\\})"
              + "|(\\[[^\\}\\]\\{\\[,]+(,[^\\}\\]\\{\\[,]+)*\\])")) {
        System.err.println("Argument for --domain-add needs to be a nonempty set "
            + "of elements enclosed in {} or [] (where elements are not allowed to have braces {}[])");
        return false;
      }
      
      // Split into list of elements
      String[] temp = value.replaceAll("\\{|\\}|\\[|\\]", "").split(",", 0);
      domainNames.put(entry.getKey(), new ArrayList<String>(Arrays.asList(temp)));
    }
    
    return true;
  }

}
