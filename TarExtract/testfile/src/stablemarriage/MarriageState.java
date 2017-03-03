/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package stablemarriage;

/**
 *
 * @author Jerry Heuring
 */
public class MarriageState {
    private java.util.ArrayList men, women;
    private int currentWoman, currentMan;
    
    public MarriageState (java.util.ArrayList<PersonInterface> men,
    		              java.util.ArrayList<PersonInterface> women,
    		              int currentWoman, int currentMan) {
    	this.men = (java.util.ArrayList)men.clone();
    	this.women = (java.util.ArrayList)women.clone();
    	this.currentWoman = currentWoman;
    	this.currentMan = currentMan;
    }

	/**
	 * @return the men
	 */
	public java.util.ArrayList getMen() {
		return men;
	}

	/**
	 * @return the women
	 */
	public java.util.ArrayList getWomen() {
		return women;
	}

	/**
	 * @return the currentWoman
	 */
	public int getCurrentWoman() {
		return currentWoman;
	}

	/**
	 * @return the currentMan
	 */
	public int getCurrentMan() {
		return currentMan;
	}
    
}
