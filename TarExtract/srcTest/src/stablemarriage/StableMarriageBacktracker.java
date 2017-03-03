/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package stablemarriage;

/**
 * <H3>Description:</H3>
 * <P>
 * This class has the backtracker built into it and will allow
 * us to use backtracking to get to a solution.
 * </P>
 * @author Jerry Heuring
 */
public class StableMarriageBacktracker {
    java.util.ArrayList<Person> men, women;
    java.util.Stack<MarriageState> stack= new java.util.Stack<MarriageState>();

    StableMarriageBacktracker () {
        Person Abe, Ben, Chuck, Dave;
        Person Amy, Barb, Corrine, Donna;
    	int currentWoman, currentMan;
    	MarriageState ms;
    	
        Abe = new Person("Abe", null, null);
        Ben = new Person("Ben", null, null);
        Chuck = new Person("Chuck", null, null);
        Dave = new Person("Dave", null, null);

        Amy = new Person("Amy", null, null, new Person[] {Abe, Ben, Chuck, Dave});
        Barb = new Person("Barb", null, null, new Person[] {Ben, Abe, Dave, Chuck});
        Corrine = new Person("Corrine", null, null, new Person[] {Dave, Chuck, Abe, Ben});
        Donna = new Person("Donna", null, null, new Person[] {Dave, Chuck, Abe, Ben});

        Abe.setPreferences(new Person[] {Amy, Barb, Corrine, Donna});
        Ben.setPreferences(new Person[]{Corrine, Barb, Amy, Donna});
        Chuck.setPreferences(new Person[]{Amy, Donna, Corrine, Barb});
        Dave.setPreferences(new Person[] {Amy, Barb, Corrine, Donna});

        men = new java.util.ArrayList<Person>();
        men.add(Abe); men.add(Ben); men.add(Chuck); men.add(Dave);
        women = new java.util.ArrayList<Person>();
        women.add(Amy); women.add(Barb); women.add(Corrine); women.add(Donna);
        
        do {

        	
        	for (currentWoman = 0; currentWoman < women.size(); currentWoman++) {
        		for (currentMan = 0; currentMan < men.size(); currentMan++) {
        			if (men.get(currentMan).isSingle()) {
        				stack.push (new MarriageState((java.util.ArrayList)men, (java.util.ArrayList)women, currentWoman, currentMan));
        				women.get(currentWoman).setSpouse(men.get(currentMan));
        				men.get(currentMan).setSpouse(women.get(currentWoman));
        				if (!isStable()) {
        					ms = stack.pop();
        					men = ms.getMen();
        					women = ms.getWomen();
        					currentWoman = ms.getCurrentWoman();
        					currentMan = ms.getCurrentMan();
        				} else {
        					currentWoman++;
        					currentMan = 0;
        				}
        			}
        		}
        	}
        } while (currentWoman < women.size());
        for (currentWoman = 0; currentWoman < women.size(); currentWoman++) {
        	System.out.println (women.get(currentWoman).getName() + " is married to " + 
        			women.get(currentWoman).getSpouse().getName());
        }
        
    }
    /**
     * This method tests if the current configuration of men and women is
     * stable. 
     * @return  true if the current configuration is stable.  false otherwise.
     */
    private boolean isStable() {
    	int woman, man;
    	
    	for (woman = 0; woman < women.size(); woman++) {
    		if (women.get(woman).isMarried()) {
    			for (man = 0; man < men.size(); man++) {
    				if (women.get(woman).getSpouse() != men.get(man)){
    					if (men.get(man).isMarried() &&
    							men.get(man).prefers((PersonInterface)women.get(woman)), (PersonInterface)men.get(man).getSpouse()
    				}
    			}
    		}
    	}
    	return true;
    }
    
    public static void main (String[] arguments) {
    	StableMarriageBacktracker bt = new StableMarriageBacktracker();
    	System.exit(0);
    }
}
