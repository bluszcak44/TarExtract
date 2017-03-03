package stablemarriage;

/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 * Description:
 * This interface is used for both men
 * and women and is used for testing
 * relationships.  The man and woman class
 * are virtually identical so that they
 * will both implement this interface.
 *
 *
 * @author Jerry Heuring
 */
public interface PersonInterface {
    /**
     * This tests if the current person prefers person one over person
     * two.
     *
     * @param one  The first person -- checking if they are prefered over the second.
     * @param two  The second person
     * @return  true if person one is preferred over person two
     */
    boolean prefers (PersonInterface one, PersonInterface two);
    /**
     *
     * @return true if t this person is married.  false otherwise.
     */
    boolean isMarried();
    /**
     *
     * @return
     */
    boolean isSingle();
    /**
     * Get the spouse of this person.  If there is no spouse the
     * method will return null.
     * @return the object representing this person's spouse or null if none.
     */
    PersonInterface getSpouse();
    /**
     * This method set's the spouse of this person.
     * @param spouse  The spouse that is to be assigned to this person
     */
    void setSpouse(PersonInterface spouse);
    /**
     * Gets this person's name.
     * @return the name assigned to this person or null if none.
     */
    String getName();
    /**
     * Sets this person's name to the string supplied.
     * @param name the name to assign to this person.
     */
    void setName(String name);
    /**
     * Gets the image that goes with this person
     * @return an image of this person or null if the property has not been set.
     */
    java.awt.Image getImage();
    /**
     * Sets the image that goes along with this person
     * @param image
     */
    void setImage(java.awt.Image image);

}
